#pragma once 

#include <array>
#include <optional>
#include <string_view>
#include <cstdint>

namespace command {

enum class Type : uint32_t {
    addWord,
    showWord,
    addTime,
    showTime,
    unknown
};

struct Entry {
    std::string_view key;
    Type value;
};

constexpr std::array<Entry, 4> table = {{
    {"add word",  Type::addWord},
    {"show words",Type::showWord},
    {"add time",  Type::addTime},
    {"show time", Type::showTime},
}};

constexpr std::optional<Type> parse(std::string_view s) {
    for (const auto& e : table) {
        if (e.key == s)
            return e.value;
    }
    return std::nullopt;
}

} // namespace command
