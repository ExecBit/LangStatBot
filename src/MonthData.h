#pragma once

#include <unordered_map>

namespace core {

class MonthData {
private:
    int month_number;                    // номер месяца (1-12)
    int days_in_month;                  // количество дней в месяце

public:
    std::unordered_map<int, int> days;  // день -> минуты
    MonthData() = default;
    MonthData(int month, int year = 2024);
    MonthData(std::unordered_map<int, int> map) : days(std::move(map)) {};

    // Добавить минуты для конкретного дня
    void addMinutes(int day, int minutes);

    // Получить минуты для конкретного дня
    int getMinutes(int day) const;

    // Установить значение для дня
    void setMinutes(int day, int minutes);

    // Получить общее количество минут за месяц
    int getTotalMinutes() const;

    // Получить среднее количество минут в день (только для дней с данными)
    double getAverageMinutes() const;

    void validateDay(int day) const;

    // Проверить, есть ли данные для дня
    bool hasDay(int day) const {
        return days.find(day) != days.end();
    }

    // Получить все дни с данными
    std::unordered_map<int, int> getAllDays() const {
        return days;
    }

    // Получить номер месяца
    int getMonthNumber() const {
        return month_number;
    }

    // Получить количество дней в месяце
    int getDaysInMonth() const {
        return days_in_month;
    }
};

};  // namespace core
