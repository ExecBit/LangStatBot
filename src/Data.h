#pragma once

#include <iomanip>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "MonthData.h"

namespace core {

struct MonthMetadata {
    int required{};
    int current{};
    int surplus{};
    int shortage{};
};

struct MonthStat {
    MonthMetadata metadata;
    std::unordered_map<int, int> days;  // day -> minutes

    std::string print() const {
        std::stringstream ss;
        ss << std::left << "required:" << std::setw(10) << metadata.required << '\n'
           << "current:" << std::setw(10) << metadata.current << '\n'
           << "surplus:" << std::setw(10) << metadata.surplus << '\n'
           << "shortage:" << std::setw(10) << metadata.shortage << '\n';

        for (const auto& [day, minutes] : days) {
            ss << day << std::setw(10) << " - " << minutes << '\n';
        }

        return ss.str();
    }
    // MonthData month;
};

struct Statistic {
    std::vector<std::string> words;
    std::unordered_map<int, std::unordered_map<int, MonthStat>> years;
};

struct Data {
    std::unique_ptr<Statistic> stat;
};

};  // namespace core
