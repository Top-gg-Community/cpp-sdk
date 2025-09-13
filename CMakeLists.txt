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
option(ENABLE_CORO "Add support for C++20 coroutines" OFF)
option(TESTING "Enable this only if you are testing the library" OFF)

if(ENABLE_API)
file(GLOB TOPGG_SOURCE_FILES src/*.cpp)
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

target_compile_definitions(topgg PRIVATE __TOPGG_BUILDING__)

if(MSVC)
target_compile_options(topgg PRIVATE /nologo $<$<CONFIG:Debug>:/diagnostics:caret /MDd /DDEBUG /D_DEBUG> $<$<CONFIG:Release>:/MD /O2 /Oi /Oy /Gy /DNDEBUG>)
else()
target_compile_options(topgg PRIVATE $<$<CONFIG:Release>:-O3> -Wall -Wextra -Wpedantic -Wformat=2 -Wnull-dereference -Wuninitialized -Wdeprecated)
endif()

target_include_directories(topgg PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${DPP_INCLUDE_DIR}
)

target_link_libraries(topgg PUBLIC ${DPP_LIBRARIES})