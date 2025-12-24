#pragma once

#include <list>
#include <map>
#include <memory>
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
    //MonthData month;
};

struct Statistic {
    std::vector<std::string> words;
    std::unordered_map<int, std::unordered_map<int, MonthStat>> years;
};

struct Data {
    std::unique_ptr<Statistic> stat;
};

};  // namespace core
