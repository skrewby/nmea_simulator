#pragma once

#include "nmea/message.hpp"
#include <cstdint>
#include <expected>
#include <string_view>
#include <unordered_map>
#include <vector>

struct Config {
    std::unordered_map<uint32_t, std::vector<nmea::NmeaMessage>> messages;
};

std::expected<Config, std::string> get_config(std::string_view config_file_path);
