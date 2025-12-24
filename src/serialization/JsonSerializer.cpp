#include "JsonSerializer.h"

#include <json/json.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <source_location>

#include "../Data.h"
#include "../MonthData.h"

namespace {

static std::optional<int> parsePositiveInt(const std::string& str) {
    printf("parsePositiveInt - input:%s\n", str.c_str());
    if (str.empty()) {
        std::cerr << "Error: " << std::source_location::current().file_name() << ":"
                  << std::source_location::current().line() << "\n";
        return std::nullopt;
    }

    for (char c : str) {
        if (c < '0' || c > '9') {
            std::cerr << "Error: " << std::source_location::current().file_name() << ":"
                      << std::source_location::current().line() << "\n";
            return std::nullopt;
        }
    }
    try {
        size_t pos;
        int val = std::stoi(str, &pos);
        if (pos != str.size() || val <= 0) {
            std::cerr << "Error: " << std::source_location::current().file_name() << ":"
                      << std::source_location::current().line() << "\n";
            return std::nullopt;
        }

        printf("parsePositiveInt - output:%d\n", val);
        return val;
    } catch (...) {
        std::cerr << "Unexpected exception in parsePositiveInt" << '\n';
        return std::nullopt;
    }
}

static std::optional<core::MonthStatMetadata> parseMonthMetadata(const Json::Value& val) {
    std::cout << "parseMonthMetadata - input: " << val << '\n';

    if (!val.isObject()) {
        std::cerr << "Error: " << std::source_location::current().file_name() << ":"
                  << std::source_location::current().line() << "\n";
        return std::nullopt;
    }
    if (!val.isMember("required") || !val["required"].isInt()) {
        std::cerr << "Error: " << std::source_location::current().file_name() << ":"
                  << std::source_location::current().line() << "\n";
        return std::nullopt;
    }
    if (!val.isMember("current") || !val["current"].isInt()) {
        std::cerr << "Error: " << std::source_location::current().file_name() << ":"
                  << std::source_location::current().line() << "\n";
        return std::nullopt;
    }
    if (!val.isMember("surplus") || !val["surplus"].isInt()) {
        std::cerr << "Error: " << std::source_location::current().file_name() << ":"
                  << std::source_location::current().line() << "\n";
        return std::nullopt;
    }
    if (!val.isMember("shortage") || !val["shortage"].isInt()) {
        std::cerr << "Error: " << std::source_location::current().file_name() << ":"
                  << std::source_location::current().line() << "\n";
        return std::nullopt;
    }

    core::MonthStatMetadata meta;
    meta.required = val["required"].asInt();
    meta.current = val["current"].asInt();
    meta.surplus = val["surplus"].asInt();
    meta.shortage = val["shortage"].asInt();
    return meta;
}

static std::optional<std::unordered_map<int, int>> parseDays(const Json::Value& val) {
    std::cout << "parseDays - input: " << val << '\n';
    const auto sl = std::source_location::current();
    if (!val.isObject()) {
        std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
        return std::nullopt;
    }

    std::unordered_map<int, int> days;
    for (const auto& key : val.getMemberNames()) {
        auto dayNum = parsePositiveInt(key);
        if (!dayNum || *dayNum > 31) {
            std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
            return std::nullopt;
        }

        if (!val[key].isInt()) {
            std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
            return std::nullopt;
        }
        days[*dayNum] = val[key].asInt();
    }
    return days;
}

std::optional<core::Statistic> parseReport(const Json::Value& root) {
    printf("parseReport\n");
    const auto sl = std::source_location::current();
    if (!root.isObject()) {
        std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
        return std::nullopt;
    }

    core::Statistic report;
    for (const auto& yearStr : root.getMemberNames()) {
        if (yearStr == "words") {
            continue;
        }


        auto year = parsePositiveInt(yearStr);
        if (!year || *year > 9999) {
            std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
            return std::nullopt;
        }

        const auto& yearObj = root[yearStr];
        if (!yearObj.isObject()) {
            std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
            return std::nullopt;
        }

        printf("BUG: input year:%d\n", *year);
        report.years.emplace_back();
        report.years.back().emplace(*year, core::MonthStat{});
        auto& months = report.years.at(*year);

        //months.reserve(12);
        for (const auto& monthStr : yearObj.getMemberNames()) {
            auto month = parsePositiveInt(monthStr);
            if (!month || *month > 12) {
                std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
                return std::nullopt;
            }

            const auto& monthObj = yearObj[monthStr];
            if (!monthObj.isObject()) {
                std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
                return std::nullopt;
            }
            if (!monthObj.isMember("metadata") || !monthObj.isMember("days")) {
                std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
                return std::nullopt;
            }

            auto metadata = parseMonthMetadata(monthObj["metadata"]);
            if (!metadata) {
                std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
                return std::nullopt;
            }

            auto days = parseDays(monthObj["days"]);
            if (!days) {
                std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
                return std::nullopt;
            }

            core::MonthStat tmp;
            tmp.metadata = std::move(*metadata);
            tmp.month = std::move(*days);

            //months.insert({*month, tmp});
            months.insert_or_assign(*month, tmp);
            // months[*month] = std::move(tmp);
        }
    }
    return report;
}
}  // namespace

namespace serialization {

bool JsonSerializer::deserialize(const std::string& src, core::Data& dst) const {
    const auto sl = std::source_location::current();
    Json::Value root;

    const auto rawJsonLength = static_cast<int>(src.length());
    JSONCPP_STRING err;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(src.c_str(), src.c_str() + rawJsonLength, &root, &err)) {
        std::cout << "error: " << err << std::endl;
        return false;
    }

    if (!root["words"].isArray() || !root.isMember("words")) {
        std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
        return false;
    };

    core::Data data;
    const auto jsonArray = root["words"];
    for (const auto& word : jsonArray) {
        if (!word.isString()) {
            std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
            return false;
        }
        data.words.push_back(word.asString());
    }

    const auto res = parseReport(root);
    if (!res) {
        std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
        return false;
    }
    data.stat = std::make_unique<core::Statistic>(*res);

    dst = std::move(data);

    printf("Serialize json data succeess\n");
    return true;
}

bool JsonSerializer::serialize(const core::Data& src, std::string& dst) const { return true;}

};  // namespace serialization
