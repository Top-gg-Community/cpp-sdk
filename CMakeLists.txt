cmake_minimum_required(VERSION 3.8.2)

project(
  topgg
  LANGUAGES CXX
  HOMEPAGE_URL "https://docs.top.gg/docs"
  DESCRIPTION "The official C++ wrapper for the Top.gg API."
)

file(GLOB TOPGG_SOURCE_FILES src/*.cpp)

add_library(topgg SHARED ${TOPGG_SOURCE_FILES})

set_target_properties(topgg PROPERTIES
  OUTPUT_NAME           topgg
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON
)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(DPP REQUIRED)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Oi /Oy /Gy")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wpedantic -Wformat=2 -Wnull-dereference -Wuninitialized -Wdeprecated")
endif()

if(WIN32)
target_compile_definitions(topgg PRIVATE TOPGG_SHARED)
target_compile_definitions(topgg PRIVATE __TOPGG_BUILDING__)
endif()

target_include_directories(topgg PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${DPP_INCLUDE_DIR}
)

target_link_libraries(topgg ${DPP_LIBRARIES})