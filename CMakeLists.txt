cmake_minimum_required(VERSION 3.8.2)

project(
  topgg
  LANGUAGES CXX
  HOMEPAGE_URL "https://docs.top.gg/docs"
  DESCRIPTION "The official C++ wrapper for the Top.gg API."
)

file(GLOB TOPGG_SOURCE_FILES src/dpp/*.cpp)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Oi /Oy /Gy")
endif()

add_library(topgg_static STATIC ${TOPGG_SOURCE_FILES})
add_compile_definitions(DPP_STATIC)
target_include_directories(topgg_static PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(topgg_static PROPERTIES
  OUTPUT_NAME           topgg_static
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON
)