#include "common/def.h"
#include "core/Data.h"
#include "core/IBot.h"
#include "fsm/StateMachine.h"
#include "io_interface/IDataManager.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

struct MockBot : core::IBot {
    MOCK_METHOD(void, sendMessage,
                (std::int64_t chatId, const std::string& text, def::KeyboardType), (override));
    MOCK_METHOD(void, sendDocument,
                (std::int64_t chatId, const def::DocumentData& doc, def::KeyboardType), (override));
    MOCK_METHOD(void, deleteWebhook, (), (override));
    MOCK_METHOD(void, deleteMyCommands, (), (override));
    MOCK_METHOD(std::string, username, (), (override));
    MOCK_METHOD(void, setCommands, (), (override));
    MOCK_METHOD(void, setCallback, (fsm::StateMachine & stateMachine), (override));
    MOCK_METHOD(void, startLongPoll, (), (override));
};

struct MockDataInterface : io_interface::IDataManager {
    MOCK_METHOD(bool, load, (std::string_view src, core::Data& dst), (override));
    MOCK_METHOD(std::string, save, (const core::Data& src, std::string_view path), (override));
};

struct StateMachineTest : ::testing::Test {
  protected:
    static void SetUpTestSuite() { spdlog::set_level(spdlog::level::off); }

  public:
    StateMachineTest()
        : bot(std::make_unique<MockBot>()), dataMgr(std::make_unique<MockDataInterface>()),
          sm(bot.get(), data, dataMgr.get()) {
        data.stat = std::make_unique<core::Statistic>();
    }

    std::unique_ptr<MockBot> bot;
    std::unique_ptr<MockDataInterface> dataMgr;
    core::Data data;
    fsm::StateMachine sm;
};

TEST_F(StateMachineTest, ShowWordEmpty) {

    using ::testing::_;
    using ::testing::Eq;

    EXPECT_CALL(*bot,
                sendMessage(0, "Dictionary is empty", def::KeyboardType::keyboardChooseCommands))
        .Times(1);

    {
        core::Message msg{.chat_id = 0, .text = "show words"};
        sm.execute(msg);
    }

    ASSERT_EQ(data.stat->words.size(), 0);
}

TEST_F(StateMachineTest, ShowWord) {

    using ::testing::_;
    using ::testing::Eq;

    EXPECT_CALL(*bot, sendMessage(0, "Enter text", def::KeyboardType::none)).Times(1);
    EXPECT_CALL(*bot, sendMessage(0, "ok", def::KeyboardType::keyboardChooseCommands)).Times(1);
    EXPECT_CALL(*bot, sendMessage(0, "testword", def::KeyboardType::keyboardChooseCommands)).Times(1);

    {
        core::Message msg{.chat_id = 0, .text = "add word"};
        sm.execute(msg);
    }

    {
        core::Message msg{.chat_id = 0, .text = "testword"};
        sm.execute(msg);
    }

    ASSERT_EQ(data.stat->words.size(), 1);
    EXPECT_EQ(data.stat->words[0], "testword");

    {
        core::Message msg{.chat_id = 0, .text = "show words"};
        sm.execute(msg);
    }

    ASSERT_EQ(data.stat->words.size(), 1);
}

TEST_F(StateMachineTest, AddWordFlow) {

    using ::testing::_;
    using ::testing::Eq;

    EXPECT_CALL(*bot, sendMessage(0, "Enter text", def::KeyboardType::none)).Times(1);
    EXPECT_CALL(*bot, sendMessage(0, "ok", def::KeyboardType::keyboardChooseCommands)).Times(1);

    {
        core::Message msg{.chat_id = 0, .text = "add word"};
        sm.execute(msg);
    }

    {
        core::Message msg{.chat_id = 0, .text = "testword"};
        sm.execute(msg);
    }

    ASSERT_EQ(data.stat->words.size(), 1);
    EXPECT_EQ(data.stat->words[0], "testword");
}
