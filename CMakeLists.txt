cmake_minimum_required(VERSION 3.10)
project(lang_time_tracker_bot)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" OFF)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(lang_time_tracker_bot)

target_sources(lang_time_tracker_bot
    PRIVATE
        src/main.cpp
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()


