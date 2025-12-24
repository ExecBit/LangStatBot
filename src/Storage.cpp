#include "Storage.h"

#include <fstream>
#include <iostream>
#include <source_location>
#include <sstream>

namespace io_interface {

bool Storage::read(std::string_view path, std::string& file) {
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

    std::stringstream ss;
    ss << input.rdbuf();
    file.clear();
    file.append(ss.str());

    printf("read data succeess\n");
    return true;
}

bool Storage::write(std::string_view path, std::string_view file) {
    const auto sl = std::source_location::current();

    std::ofstream output{path.data()};
    if (!output) {
        std::cerr << "Error, can't create file '" << path << "'\n";
        std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
        return false;
    }
    output.clear();
    output << file;

    return true;
}

};  // namespace io_interface
