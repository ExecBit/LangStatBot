
#pragma once

#include <string>
#include <tgbot/tgbot.h>
#include <vector>

namespace def {

enum class KeyboardType : std::uint8_t { keyboardWithLayout, keyboardChooseMonth, none };

struct DocumentData {
    std::string data;
    std::string mimeType;
    std::string fileName;
};

inline std::vector<std::vector<std::string>> keyboardWithLayoutVector = {
    {"option", "edit time"},
    {"dump data", "recieve file"},
    {"show words", "show time"},
    {"add word", "add time"}};

inline std::vector<std::vector<std::string>> keyboardChooseMonthVector = {
    {"1", "2", "3", "4", "5", "6"},
    {"7", "8", "9", "10", "11", "12"},
};

inline void createKeyboard(const std::vector<std::vector<std::string>>& buttonLayout,
                           TgBot::ReplyKeyboardMarkup::Ptr& kb) {
    for (size_t i = 0; i < buttonLayout.size(); ++i) {
        std::vector<TgBot::KeyboardButton::Ptr> row;
        for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
            TgBot::KeyboardButton::Ptr button(new TgBot::KeyboardButton);
            button->text = buttonLayout[i][j];
            button->requestContact = false;
            button->requestLocation = false;
            row.push_back(button);
        }
        kb->keyboard.push_back(row);
    }
}

}; // namespace def
