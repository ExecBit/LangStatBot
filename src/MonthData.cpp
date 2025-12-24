#include "MonthData.h"

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool isLeapYear(int year) { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); }

// Определяем количество дней в месяце
int calculateDaysInMonth(int month, int year) {
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Номер месяца должен быть от 1 до 12");
    }

    const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Проверка на високосный год для февраля
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }

    return days[month - 1];
}

}  // namespace

namespace core {

MonthData::MonthData(int month, int year) : month_number(month) {
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Номер месяца должен быть от 1 до 12");
    }
    days_in_month = ::calculateDaysInMonth(month, year);
}

void MonthData::addMinutes(int day, int minutes) {
    validateDay(day);
    days[day] += minutes;
}

// Получить минуты для конкретного дня
int MonthData::getMinutes(int day) const {
    validateDay(day);

    auto it = days.find(day);
    if (it != days.end()) {
        return it->second;
    }
    return 0;  // Если день не найден, возвращаем 0
}

// Установить значение для дня
void MonthData::setMinutes(int day, int minutes) {
    validateDay(day);

    if (minutes == 0) {
        // Удаляем запись если минуты = 0
        days.erase(day);
    } else {
        days[day] = minutes;
    }
}

// Получить общее количество минут за месяц
int MonthData::getTotalMinutes() const {
    int total = 0;
    for (const auto& [day, minutes] : days) {
        total += minutes;
    }
    return total;
}

// Получить среднее количество минут в день (только для дней с данными)
double MonthData::getAverageMinutes() const {
    if (days.empty()) {
        return 0.0;
    }
    return static_cast<double>(getTotalMinutes()) / days.size();
}

void MonthData::validateDay(int day) const {
    if (day < 1 || day > days_in_month) {
        throw std::out_of_range("День " + std::to_string(day) + " выходит за пределы месяца " +
                                std::to_string(month_number));
    }
}


};  // namespace core
