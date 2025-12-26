#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

namespace command {

enum class Type : uint32_t { addWord, showWord, addTime, showTime, dumpData, recieveFile, unknown };

struct Entry {
    std::string_view key;
    Type value;
};

constexpr std::array<Entry, static_cast<uint32_t>(Type::unknown)> table = {{
    {"add word", Type::addWord},
    {"show words", Type::showWord},
    {"add time", Type::addTime},
    {"show time", Type::showTime},
    {"dump data", Type::dumpData},
    {"recieve file", Type::recieveFile},
}};

constexpr Type parse(std::string_view s) {
    for (const auto& e : table) {
        if (e.key == s) return e.value;
    }
    return Type::unknown;
}

}  // namespace command
