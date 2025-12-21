#pragma once

#include <list>
#include <map>
#include <memory>
#include "MonthData.h"

namespace core {

struct MonthStat {
    size_t required{0};
    size_t current{0};
    size_t surplus{0};
    size_t shortage{0};

    MonthData month;
};

struct Statistic {
    size_t total{0};
    std::list<MonthStat> months;
};

struct Data {
    std::list<std::string> words;
    std::unique_ptr<Statistic> stat;
};

};  // namespace core
