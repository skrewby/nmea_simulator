#include "config.hpp"
#include "nmea/message.hpp"
#include <algorithm>
#include <charconv>
#include <cstdint>
#include <expected>
#include <format>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#define TOML_IMPLEMENTATION
#include <toml++/toml.hpp>

struct string_hash {
    using is_transparent = void;

    size_t operator()(std::string_view sv) const { return std::hash<std::string_view>{}(sv); }
};

using field_fn =
    std::function<std::expected<void, std::string>(nmea::NmeaMessage &, const toml::node &)>;
using field_map = std::unordered_map<std::string, field_fn, string_hash, std::equal_to<>>;

struct MessageParser {
    std::function<nmea::NmeaMessage()> factory;
    field_map fields;
};

template <typename Msg, typename T> field_fn field(T Msg::*member) {
    return [member](nmea::NmeaMessage &msg,
                    const toml::node &node) -> std::expected<void, std::string> {
        auto val = node.value<T>();
        if (!val) {
            return std::unexpected("type mismatch");
        }
        std::get<Msg>(msg).*member = *val;
        return {};
    };
}

template <typename Msg> MessageParser make_parser(field_map fields) {
    return {[]() -> nmea::NmeaMessage { return Msg{}; }, std::move(fields)};
}

// clang-format off
static const std::unordered_map<uint32_t, MessageParser> nmea_messages = {
    {nmea::pgn::COG_SOG, make_parser<nmea::message::CogSog>({
        {"cog",           field(&nmea::message::CogSog::cog)},
        {"sog",           field(&nmea::message::CogSog::sog)},
        {"cog_reference", field(&nmea::message::CogSog::cog_reference)},
    })},
    {nmea::pgn::TEMPERATURE, make_parser<nmea::message::Temperature>({
        {"actual_temperature", field(&nmea::message::Temperature::actual_temperature)},
        {"set_temperature",    field(&nmea::message::Temperature::set_temperature)},
        {"instance",           field(&nmea::message::Temperature::instance)},
        {"source",             field(&nmea::message::Temperature::source)},
    })},
};
// clang-format on

using toml_arrays_vec = std::vector<std::pair<std::string_view, const toml::array *>>;

static toml_arrays_vec collect_arrays(const toml::table &section) {
    toml_arrays_vec arrays;
    for (auto &[key, node] : section) {
        if (auto *arr = node.as_array()) {
            arrays.emplace_back(key.str(), arr);
        }
    }
    return arrays;
}

static std::expected<std::vector<nmea::NmeaMessage>, std::string>
get_messages(const MessageParser &parser, const toml_arrays_vec &arrays, uint32_t pgn) {
    size_t count = arrays[0].second->size();
    bool sizes_match = std::all_of(arrays.begin(), arrays.end(),
                                   [count](const auto &p) { return p.second->size() == count; });
    if (!sizes_match) {
        return std::unexpected(std::format("[{}] all fields must have the same length", pgn));
    }

    std::vector<nmea::NmeaMessage> messages;
    messages.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        auto msg = parser.factory();
        for (auto &[field_name, arr] : arrays) {
            auto field_parser = parser.fields.find(field_name);
            if (field_parser == parser.fields.end()) {
                return std::unexpected(std::format("[{}] unknown field '{}'", pgn, field_name));
            }
            const auto &field_parser_fn = field_parser->second;
            if (auto result = field_parser_fn(msg, (*arr)[i]); !result) {
                return std::unexpected(
                    std::format("[{}]['{}'] {}", pgn, field_name, result.error()));
            }
        }
        messages.push_back(std::move(msg));
    }
    return messages;
}

static std::expected<Config, std::string> parse_config(toml::table tbl) {
    Config config;

    for (auto &[key, node] : tbl) {
        uint32_t pgn{};
        auto [ptr, ec] =
            std::from_chars(key.str().data(), key.str().data() + key.str().size(), pgn);
        if (ec != std::errc{}) {
            continue;
        }

        auto message_definition = nmea_messages.find(pgn);
        if (message_definition == nmea_messages.end()) {
            continue;
        }
        const auto &message_parser = message_definition->second;

        auto *section = node.as_table();
        if (!section) {
            continue;
        }

        auto arrays = collect_arrays(*section);
        if (arrays.empty()) {
            continue;
        }

        auto msgs = get_messages(message_parser, arrays, pgn);
        if (!msgs) {
            return std::unexpected(msgs.error());
        }

        for (auto &msg : *msgs) {
            config.messages[pgn].push_back(std::move(msg));
        }
    }

    return config;
}

std::expected<Config, std::string> get_config(std::string_view config_file_path) {
    auto result = toml::parse_file(config_file_path);
    if (!result) {
        auto desc = result.error().description();
        auto src = result.error().source().begin;
        auto err = std::format("at ({}:{}) {}", src.line, src.column, desc);
        return std::unexpected(err);
    }

    return parse_config(std::move(result).table());
}
