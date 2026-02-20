#include "CLI11.hpp"
#include "config.hpp"
#include "nmea/connection.hpp"
#include "nmea/definitions.hpp"
#include "nmea/device.hpp"
#include <algorithm>
#include <cstdlib>
#include <expected>
#include <format>
#include <print>
#include <string_view>

std::expected<void, std::string> start(std::string_view interface, const Config &config) {
    auto conn = nmea::connect(interface);
    if (!conn) {
        auto err = std::format("Error on connection: {}", conn.error());
        return std::unexpected(err);
    }

    nmea::DeviceName name{
        .unique_number = 120,
        .manufacturer_code = ManufacturerCode::ACTISENSE,
        .device_instance_lower = 0,
        .device_instance_upper = 0,
        .device_function = device_function::ATMOSPHERIC,
        .system_instance = 0,
        .industry_group = IndustryCode::MARINE,
        .arbitrary_address_capable = true,
    };

    nmea::Device device(*conn);
    auto result = device.claim(name).get();
    if (!result) {
        auto err = std::format("Failed to claim address: {}", result.error());
        return std::unexpected(err);
    }

    size_t max_count = 0;
    for (const auto &[pgn, messages] : config.messages) {
        max_count = std::max(max_count, messages.size());
    }

    for (size_t i = 0; i < max_count; ++i) {
        for (const auto &[pgn, messages] : config.messages) {
            if (i >= messages.size()) {
                continue;
            }
            auto send_res = device.send(messages[i]);
            if (!send_res) {
                return std::unexpected(std::format("[{}] send error: {}", pgn, send_res.error()));
            }
        }
    }

    return {};
}

int main(int argc, char **argv) {
    CLI::App app{"NMEA Simulator"};
    argv = app.ensure_utf8(argv);

    std::string can_interface;
    app.add_option("-c,--can", can_interface, "CAN interface to connect to NMEA2000 network")
        ->default_val("can0");
    std::string config_file_path;
    app.add_option("-C,--config", config_file_path, "Path to test config file")->required();
    CLI11_PARSE(app, argc, argv);

    auto config_res = get_config(config_file_path);
    if (!config_res) {
        std::println("Error reading config file: {}", config_res.error());
        return EXIT_FAILURE;
    }
    auto config = std::move(*config_res);

    auto res = start(can_interface, config);
    if (!res) {
        std::println("{}", res.error());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
