# Тесты для lang_time_tracker_bot
find_package(GTest REQUIRED)

add_executable(lang_time_tracker_bot_tests
    test_main.cpp
)

target_link_libraries(lang_time_tracker_bot_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

add_test(NAME lang_time_tracker_bot_tests COMMAND lang_time_tracker_bot_tests)

