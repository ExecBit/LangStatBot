#pragma once

#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "MonthData.h"

namespace core {

struct MonthStatMetadata {
    MonthStatMetadata() = default;
    size_t required{0};
    size_t current{0};
    size_t surplus{0};
    size_t shortage{0};
};

struct MonthStat {
    MonthStat() = default;
    MonthStatMetadata metadata;
    MonthData month;
};

struct Statistic {
    size_t total{0};
    // std::list<MonthStat> months;
    using mapMonth = std::unordered_map<size_t, MonthStat>;
    std::vector<mapMonth> years;
    // std::unordered_map<size_t, MonthStat> months;
};

struct Data {
    std::list<std::string> words;
    std::unique_ptr<Statistic> stat;
};

};  // namespace core
