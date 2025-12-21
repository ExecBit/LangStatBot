cmake_minimum_required(VERSION 3.22)
project(lang_time_tracker_bot)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" OFF)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

#find_package(jsoncpp CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
#find_package(jsoncpp)

include(FetchContent)
FetchContent_Declare(
    tgbot-cpp
    GIT_REPOSITORY https://github.com/reo7sp/tgbot-cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(tgbot-cpp)

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
        jsoncpp
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()


