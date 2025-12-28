#pragma once

#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace core {

struct MonthMetadata {
    std::chrono::minutes quota{};
    std::chrono::minutes total{};
    std::chrono::minutes overfulfillment{};
    std::chrono::minutes debt{};
};

struct MonthStat {
private:
    using MapData = std::map<int, std::chrono::minutes>;
    MonthMetadata _metadata;
    MapData _days;  // day -> minutes
public:
    // TODO убрать эти методы
    MonthMetadata& metadata() { return _metadata; }
    MapData& days() { return _days; }

    const MonthMetadata& readMetadata() const { return _metadata; }
    const MapData& readDays() const { return _days; }

    std::string print() const {
        std::stringstream ss;
        ss << std::right << "Quota: " << _metadata.quota << '\n'
           << "Overfulfillment (debt): " << _metadata.overfulfillment << " (" << _metadata.debt << ")" << '\n';

        for (const auto& [day, minutes] : _days) {
            ss << std::setw(2) << std::setfill('0') << day << " - " << minutes << '\n';
        }

        const auto hours = std::chrono::duration_cast<std::chrono::hours>(_metadata.total);
        const auto minutes = _metadata.total - hours;
        ss << "Total: " << hours << " " << minutes << '\n';

        return ss.str();
    }
    void edit(int day, std::chrono::minutes value) {
        _days[day] = value;
        update();
    }

    std::chrono::minutes add(int day, std::chrono::minutes value) {
        _days[day] += value;
        update();
        return _days[day];
    }

    void update() {
        using namespace std::chrono_literals;

        const auto totalMinutes = std::ranges::fold_left(_days | std::views::values, 0min, std::plus{});
        const auto plannedMinutes = std::ranges::distance(_days) * _metadata.quota;

        const auto balance = totalMinutes - plannedMinutes;

        _metadata.total = totalMinutes;
        _metadata.overfulfillment = std::max(balance, 0min);
        _metadata.debt = std::max(-balance, 0min);
    }
};

struct Statistic {
    std::vector<std::string> words;
    std::unordered_map<int, std::unordered_map<int, MonthStat>> years;
};

struct Data {
    std::unique_ptr<Statistic> stat;
};

};  // namespace core
