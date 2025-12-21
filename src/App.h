#pragma once

#include <tgbot/tgbot.h>

#include <memory>

#include "IDataManager.h"
#include "BotEntity.h"

namespace io_interface {
class IDataManager;
};

namespace core {

class App {
public:
    explicit App(std::string_view token, std::unique_ptr<io_interface::IDataManager> ptr)
        : m_bot{std::move(token)}
        , m_dataMgr{std::move(ptr)} {}
    bool initBot();

    void start();

private:
    BotEntity m_bot;
    std::unique_ptr<io_interface::IDataManager> m_dataMgr;
};

};  // namespace core
