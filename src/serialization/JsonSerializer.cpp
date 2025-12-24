#include "JsonSerializer.h"

#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "../Data.h"
#include "../MonthData.h"

namespace core {

using json = nlohmann::json;

void to_json(json& j, const core::MonthMetadata& m) {
    j = {{"required", m.required}, {"current", m.current}, {"surplus", m.surplus}, {"shortage", m.shortage}};
}

void from_json(const json& j, core::MonthMetadata& m) {
    j.at("required").get_to(m.required);
    j.at("current").get_to(m.current);
    j.at("surplus").get_to(m.surplus);
    j.at("shortage").get_to(m.shortage);
}

void to_json(json& j, const core::MonthStat& m) {
    json days;
    for (auto& [day, minutes] : m.days) {
        days[std::to_string(day)] = minutes;
    }

    j = {{"metadata", m.metadata}, {"days", days}};
}

void from_json(const json& j, core::MonthStat& m) {
    j.at("metadata").get_to(m.metadata);

    m.days.clear();
    for (auto& [dayStr, value] : j.at("days").items()) {
        m.days.emplace(std::stoi(dayStr), value.get<int>());
    }
}

void to_json(json& j, const core::Statistic& s) {
    j["words"] = s.words;

    for (auto& [year, months] : s.years) {
        json monthsJson;
        for (auto& [month, stat] : months) {
            monthsJson[std::to_string(month)] = stat;
        }
        j[std::to_string(year)] = monthsJson;
    }
}

void from_json(const json& j, core::Statistic& s) {
    s.words = j.at("words").get<std::vector<std::string>>();

    s.years.clear();
    for (auto& [key, value] : j.items()) {
        if (key == "words") continue;

        int year = std::stoi(key);
        auto& months = s.years[year];

        for (auto& [monthStr, monthJson] : value.items()) {
            months.emplace(std::stoi(monthStr), monthJson.get<core::MonthStat>());
        }
    }
}

}  // namespace

namespace serialization {

bool JsonSerializer::deserialize(const std::string& src, core::Data& dst) const {
    auto j = nlohmann::json::parse(src);
    //auto stat = j.get<core::Statistic>();

    dst.stat = std::make_unique<core::Statistic>(j.get<core::Statistic>());
    //dst.stat = std::move(stat);

    printf("Serialize json data succeess\n");
    return true;
}

bool JsonSerializer::serialize(const core::Data& src, std::string& dst) const {
    core::Statistic stat = *src.stat.get();
    nlohmann::json out = stat;
    // dst << out.dump(2).append(dst);
    dst.clear();
    dst.append(out.dump(2));
    std::cout << "\nSerialized back to JSON:\n";

    return true;
}

};  // namespace serialization
