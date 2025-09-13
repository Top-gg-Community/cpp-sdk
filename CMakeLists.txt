cmake_minimum_required(VERSION 3.15)

project(
  topgg
  LANGUAGES CXX
  HOMEPAGE_URL "https://docs.top.gg/docs"
  DESCRIPTION "A simple API wrapper for Top.gg written in C++."
)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ENABLE_API "Build primary API support" ON)
option(ENABLE_CPP_HTTPLIB_WEBHOOKS "Build support for webhooks via cpp-httplib" OFF)
option(ENABLE_DROGON_WEBHOOKS "Build support for webhooks via drogon" OFF)
option(ENABLE_CORO "Add support for C++20 coroutines" OFF)
option(TESTING "Enable this only if you are testing the library" OFF)

if(ENABLE_API)
file(GLOB TOPGG_SOURCE_FILES src/*.cpp)
endif()

if(ENABLE_CPP_HTTPLIB_WEBHOOKS)
set(TOPGG_SOURCE_FILES ${TOPGG_SOURCE_FILES} src/webhooks/cpp-httplib.cpp src/webhooks/models.cpp)
elseif(ENABLE_DROGON_WEBHOOKS)
set(TOPGG_SOURCE_FILES ${TOPGG_SOURCE_FILES} src/webhooks/drogon.cpp src/webhooks/models.cpp)
endif()

if(BUILD_SHARED_LIBS)
add_library(topgg SHARED ${TOPGG_SOURCE_FILES})

if(WIN32)
target_sources(topgg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/topgg.rc)
target_compile_definitions(topgg PRIVATE __TOPGG_BUILDING_DLL__)
endif()
else()
add_library(topgg STATIC ${TOPGG_SOURCE_FILES})

if(WIN32)
target_compile_definitions(topgg PUBLIC DPP_STATIC TOPGG_STATIC)
endif()
endif()

if(ENABLE_CORO)
set(TOPGG_CXX_STANDARD 20)
target_compile_definitions(topgg PUBLIC DPP_CORO=ON)
else()
set(TOPGG_CXX_STANDARD 17)
endif()

if(TESTING)
target_compile_definitions(topgg PUBLIC __TOPGG_TESTING__)
endif()

set_target_properties(topgg PROPERTIES
  CXX_STANDARD          ${TOPGG_CXX_STANDARD}
  CXX_STANDARD_REQUIRED ON
)

if(ENABLE_API)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(DPP REQUIRED)
endif()

if(ENABLE_CPP_HTTPLIB_WEBHOOKS)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-httplib/cpp-httplib.h")
execute_process(COMMAND git clone https://github.com/yhirose/cpp-httplib.git --depth 1 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-httplib")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-httplib")
endif()
file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib/httplib.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-httplib/cpp-httplib.h")
file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann/json.hpp")
execute_process(COMMAND git clone https://github.com/nlohmann/json.git --depth 1 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann")
endif()
file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/json/single_include/nlohmann/json.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann/json.hpp")
file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/json")
endif()
endif()

if(ENABLE_DROGON_WEBHOOKS)
if(WIN32)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/conan_toolchain.cmake")

include(${CMAKE_CURRENT_SOURCE_DIR}/conan_toolchain.cmake)
endif()

set(DROGON_LIBRARY drogon)

set(
  TRANTOR_INCLUDE_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}/deps/drogon/trantor"
  "${CMAKE_BINARY_DIR}/deps/drogon/trantor/exports"
)

set(
  DROGON_INCLUDE_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}/deps/drogon/lib/inc"
  "${CMAKE_CURRENT_SOURCE_DIR}/deps/drogon/orm_lib/inc"
  "${CMAKE_CURRENT_SOURCE_DIR}/deps/drogon/nosql_lib/redis/inc"
  "${CMAKE_BINARY_DIR}/deps/drogon/exports"
)

set(BUILD_CTL OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_BROTLI OFF)
set(BUILD_YAML_CONFIG OFF)
set(USE_SUBMODULE ON)

add_subdirectory(deps/drogon)

target_compile_definitions(topgg PUBLIC __TOPGG_DROGON_WEBHOOKS__)

if(WIN32)
target_compile_definitions(topgg PRIVATE _CRT_SECURE_NO_WARNINGS)
cmake_policy(SET CMP0091 NEW)
endif()
endif()

target_compile_definitions(topgg PRIVATE __TOPGG_BUILDING__)

if(MSVC)
target_compile_options(topgg PRIVATE /nologo $<$<CONFIG:Debug>:/diagnostics:caret /MDd /DDEBUG /D_DEBUG> $<$<CONFIG:Release>:/MD /O2 /Oi /Oy /Gy /DNDEBUG>)
else()
target_compile_options(topgg PRIVATE $<$<CONFIG:Release>:-O3> -Wall -Wextra -Wpedantic -Wformat=2 -Wnull-dereference -Wuninitialized -Wdeprecated)
endif()

target_include_directories(topgg PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${DPP_INCLUDE_DIR}
  ${JSONCPP_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIR}
  ${TRANTOR_INCLUDE_DIR}
  ${DROGON_INCLUDE_DIR}
)

target_link_libraries(topgg PUBLIC ${DPP_LIBRARIES} ${JSONCPP_LIBRARIES} ${DROGON_LIBRARY})