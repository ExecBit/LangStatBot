add_executable(lang_time_tracker_bot)

target_sources(lang_time_tracker_bot
    PRIVATE
        main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        lang_time_tracker_lib
        TgBot
)

