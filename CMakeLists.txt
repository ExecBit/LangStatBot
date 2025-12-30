cmake_minimum_required(VERSION 3.22)
project(lang_time_tracker_bot)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

add_compile_options(-Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wno-unused-parameter
    -Wno-unused-value
    -Wno-missing-field-initializers
)

find_package(spdlog REQUIRED)

if (ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if (CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
    else()
        message(WARNING "clang-tidy not found")
    endif()
endif()

include(FetchContent)
FetchContent_Declare(
    tgbot-cpp
    GIT_REPOSITORY https://github.com/reo7sp/tgbot-cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(tgbot-cpp)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG master
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(lang_time_tracker_bot)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c src/*.h src/*.hpp)

target_sources(lang_time_tracker_bot
    PRIVATE
        ${SOURCES}
        #src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        TgBot
        nlohmann_json
        spdlog::spdlog_header_only
)

if (ENABLE_ASAN)
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()


