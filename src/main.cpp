#include "CLI11.hpp"
#include "config.hpp"
#include "nmea/connection.hpp"
#include "nmea/definitions.hpp"
#include "nmea/device.hpp"
#include <algorithm>
#include <cstdlib>
#include <expected>
#include <fcntl.h>
#include <format>
#include <print>
#include <string_view>
#include <termios.h>
#include <unistd.h>
#include <unordered_map>

static std::expected<speed_t, std::string> get_baud_rate(int baud) {
    static const std::unordered_map<int, speed_t> baud_map = {
        {  4800,   B4800},
        {  9600,   B9600},
        { 19200,  B19200},
        { 38400,  B38400},
        { 57600,  B57600},
        {115200, B115200},
    };
    auto it = baud_map.find(baud);
    if (it == baud_map.end()) {
        return std::unexpected(std::format("Unsupported baud rate: {}", baud));
    }
    return it->second;
}

static std::expected<nmea::Device, std::string> open_device(std::string_view interface) {
    auto conn = nmea::connect(interface);
    if (!conn) {
        return std::unexpected(std::format("Error on connection: {}", conn.error()));
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
        return std::unexpected(std::format("Failed to claim address: {}", result.error()));
    }
    return device;
}

static std::expected<int, std::string> open_serial(std::string_view port, speed_t baud) {
    int fd = open(port.data(), O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        return std::unexpected(std::format("Failed to open '{}': {}", port, errno));
    }

    struct termios tty {};

    tcgetattr(fd, &tty);
    cfsetispeed(&tty, baud);
    cfmakeraw(&tty);
    tty.c_cc[VTIME] = 5;
    tty.c_cc[VMIN] = 0;
    tcsetattr(fd, TCSANOW, &tty);
    return fd;
}

static std::expected<void, std::string> send_messages(nmea::Device &device, const Config &config,
                                                      int serial_fd) {
    size_t max_count = 0;
    for (const auto &[pgn, messages] : config.messages) {
        max_count = std::max(max_count, messages.size());
    }

    for (size_t i = 0; i < max_count; ++i) {
        std::println("------------------------------ {:03} -------------------------------", i + 1);
        std::println("============================ NMEA2000 ============================");
        for (const auto &[pgn, messages] : config.messages) {
            if (i >= messages.size()) {
                continue;
            }
            auto send_res = device.send(messages[i]);
            if (!send_res) {
                return std::unexpected(std::format("[{}] send error: {}", pgn, send_res.error()));
            }
            std::println("{}", messages[i]);
        }
        if (serial_fd >= 0) {
            std::println("============================ NMEA0183 ============================");
            char buf[256];
            ssize_t n;
            while ((n = read(serial_fd, buf, sizeof(buf))) > 0) {
                std::print("{}", std::string_view(buf, n));
            }
        }
        std::println("==================================================================");
        std::println("");
    }
    return {};
}

std::expected<void, std::string> start(std::string_view interface, const Config &config,
                                       std::string_view serial_port, speed_t baud) {
    auto device = open_device(interface);
    if (!device) {
        return std::unexpected(device.error());
    }

    int serial_fd = -1;
    if (!serial_port.empty()) {
        auto fd = open_serial(serial_port, baud);
        if (!fd) {
            return std::unexpected(fd.error());
        }
        serial_fd = *fd;
    }

    auto result = send_messages(*device, config, serial_fd);
    if (serial_fd >= 0) {
        close(serial_fd);
    }
    return result;
}

int main(int argc, char **argv) {
    CLI::App app{"NMEA Simulator"};
    argv = app.ensure_utf8(argv);

    std::string can_interface;
    app.add_option("-c,--can", can_interface, "CAN interface to connect to NMEA2000 network")
        ->default_val("can0");
    std::string config_file_path;
    app.add_option("-C,--config", config_file_path, "Path to test config file")->required();
    std::string serial_port;
    app.add_option("-s,--serial", serial_port, "Serial port to read responses from");
    int baud_config = 4800;
    app.add_option("-b,--baud", baud_config, "Baud rate for serial port")->default_val(4800);
    CLI11_PARSE(app, argc, argv);

    auto baud = get_baud_rate(baud_config);
    if (!baud) {
        std::println("{}", baud.error());
        return EXIT_FAILURE;
    }

    auto config_res = get_config(config_file_path);
    if (!config_res) {
        std::println("Error reading config file: {}", config_res.error());
        return EXIT_FAILURE;
    }
    auto config = std::move(*config_res);

    auto res = start(can_interface, config, serial_port, *baud);
    if (!res) {
        std::println("{}", res.error());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
