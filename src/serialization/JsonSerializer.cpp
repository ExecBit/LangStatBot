#include "JsonSerializer.h"

#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "../Data.h"
#include "../logger/Logger.h"

namespace core {

using json = nlohmann::json;

void to_json(json& j, const core::MonthMetadata& m) {
    j = {{"required", m.quota.count()},
         {"current", m.total.count()},
         {"surplus", m.overfulfillment.count()},
         {"shortage", m.debt.count()}};
}

void from_json(const json& j, core::MonthMetadata& m) {
    {
        std::chrono::minutes::rep value;
        j.at("required").get_to(value);
        m.quota = std::chrono::minutes{value};
    }

    {
        std::chrono::minutes::rep value;
        j.at("current").get_to(value);
        m.total = std::chrono::minutes{value};
    }

    {
        std::chrono::minutes::rep value;
        j.at("surplus").get_to(value);
        m.overfulfillment = std::chrono::minutes{value};
    }

    {
        std::chrono::minutes::rep value;
        j.at("shortage").get_to(value);
        m.debt = std::chrono::minutes{value};
    }
}

void to_json(json& j, const core::MonthStat& m) {
    json days;
    for (auto& [day, minutes] : m.readDays()) {
        days[std::to_string(day)] = minutes.count();
    }

    j = {{"metadata", m.readMetadata()}, {"days", days}};
}

void from_json(const json& j, core::MonthStat& m) {
    j.at("metadata").get_to(m.metadata());

    m.days().clear();
    for (auto& [dayStr, value] : j.at("days").items()) {
        m.days().emplace(std::stoi(dayStr), value.get<int>());
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

}  // namespace core

namespace serialization {

bool JsonSerializer::deserialize(const std::string& src, core::Data& dst) const {
    SPDLOG_INFO("Start deserialize from json");
    auto j = nlohmann::json::parse(src);
    // auto stat = j.get<core::Statistic>();

    dst.stat = std::make_unique<core::Statistic>(j.get<core::Statistic>());
    // dst.stat = std::move(stat);

    SPDLOG_INFO("Deserialize from json is success");
    return true;
}

bool JsonSerializer::serialize(const core::Data& src, std::string& dst) const {
    SPDLOG_INFO("Start serialize to json");
    core::Statistic stat = *src.stat.get();
    nlohmann::json out = stat;
    // dst << out.dump(2).append(dst);
    dst.clear();
    dst.append(out.dump(2));
    SPDLOG_INFO("Serialize to json is success");
    return true;
}

};  // namespace serialization
