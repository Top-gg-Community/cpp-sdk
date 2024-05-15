cmake_minimum_required(VERSION 3.8.2)

project(topgg)

file(GLOB TOPGG_SOURCE_FILES src/dpp/*.cpp)

add_library(topgg_static STATIC ${TOPGG_SOURCE_FILES})
target_compile_definitions(topgg_static PUBLIC DPP_STATIC)
target_include_directories(topgg_static PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(topgg_static PROPERTIES
  OUTPUT_NAME           topgg_static
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON
)