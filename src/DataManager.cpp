#include "DataManager.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <source_location>

#include "Data.h"
#include "MonthData.h"

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
        std::cout << "check bug: " << yearStr << '\n';
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

        report.years.reserve(12);
        auto& months = report.years[*year];
        months.reserve(12);
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

            months.insert({*month, tmp});
            // months[*month] = std::move(tmp);
        }
    }
    return report;
}
}  // namespace

namespace io_interface {

bool DataManager::load(std::string_view path) {
    const auto sl = std::source_location::current();

    std::ifstream input{path.data()};
    if (!input) {
        std::ofstream output{path.data()};
        if (!output) {
            std::cerr << "Error, can't create file '" << path << "'\n";
            std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
            return false;
        }
        output << "{fuckyou}\n";
        output.close();

        input = std::ifstream{path.data()};
        if (!input) {
            std::cerr << "Error: file is created, but cannot be readed\n";
            std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
            return false;
        }
    }

    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, input, &m_root, &errs)) {
        std::cout << errs << std::endl;
        std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
        return EXIT_FAILURE;
    }

    // std::cout << m_root << std::endl;

    printf("Load json data succeess\n");
    return true;
}

bool DataManager::save(std::string_view file, std::string_view path) {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    std::ofstream ofs{path.data()};
    root["Data"] = file.data();
    writer->write(root, &ofs);

    return EXIT_SUCCESS;
}

bool DataManager::fill(core::Data& data) {
    const auto sl = std::source_location::current();
    if (!m_root["words"].isArray() || !m_root.isMember("words")) {
        std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
        return false;
    };

    const auto jsonArray = m_root["words"];
    for (const auto& word : jsonArray) {
        if (!word.isString()) {
            std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
            return false;
        }
        data.words.push_back(word.asString());
    }

    const auto res = parseReport(m_root);
    if (!res) {
        std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
        return false;
    }
    data.stat = std::make_unique<core::Statistic>(*res);

    printf("Serialize json data succeess\n");
    return true;
};

};  // namespace io_interface
