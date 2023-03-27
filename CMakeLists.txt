cmake_minimum_required(VERSION 3.5)

project(topgg-sdk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(topgg
    src/http/client.cpp
    src/http/request.cpp
    src/http/response.cpp
    include/topgg/http/client.hpp
    include/topgg/http/request.hpp
    include/topgg/http/response.hpp
)

target_include_directories(topgg PUBLIC include)

target_link_libraries(topgg PUBLIC
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

if (MSVC)
    target_compile_options(topgg PRIVATE /W4 /WX)
else ()
    target_compile_options(topgg PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()

add_executable(votebot
    examples/votebot.cpp
)

target_link_libraries(votebot PRIVATE
    topgg
)

add_executable(leaderboard
    examples/leaderboard.cpp
)

target_link_libraries(leaderboard PRIVATE
    topgg
)

find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_subdirectory(third_party/Catch2)

# Uncomment if you want to run unit tests
# enable_testing()
# add_subdirectory(test)