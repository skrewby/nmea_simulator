#include "CLI11.hpp"
#include "nmea/connection.hpp"
#include "nmea/definitions.hpp"
#include "nmea/device.hpp"
#include <cstdlib>
#include <expected>
#include <format>
#include <print>
#include <string_view>

std::expected<void, std::string> start(std::string_view interface) {
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

    nmea::message::CogSog cogsog{
        .sid = 1,
        .cog_reference = 0,
        .cog = 0x1234 * 1e-4,
        .sog = 0x5678 * 1e-2,
    };
    auto send_res = device.send(cogsog);
    if (!send_res) {
        auto err = std::format("Error sending COGSOG message: {}", send_res.error());
        return std::unexpected(err);
    }

    nmea::message::Temperature temp{
        .sid = 2,
        .instance = 1,
        .source = 3,
        .actual_temperature = 0x1234 * 0.01,
        .set_temperature = 0x5678 * 0.01,
    };
    send_res = device.send(temp);
    if (!send_res) {
        auto err = std::format("Error sending Temperature message: {}", send_res.error());
        return std::unexpected(err);
    }

    return {};
}

int main(int argc, char **argv) {
    CLI::App app{"Actisense Gateway Tester"};
    argv = app.ensure_utf8(argv);

    std::string can_interface;
    app.add_option("-c,--can", can_interface, "CAN interface to connect to NMEA2000 network")
        ->default_val("can0");

    CLI11_PARSE(app, argc, argv);
    auto res = start(can_interface);
    if (!res) {
        std::print("{}\n", res.error());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
