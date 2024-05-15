cmake_minimum_required(VERSION 3.8.2)

project(
  topgg
  LANGUAGES CXX
  HOMEPAGE_URL "https://docs.top.gg/docs"
  DESCRIPTION "The official C++ wrapper for the Top.gg API."
)

file(GLOB TOPGG_SOURCE_FILES src/dpp/*.cpp)

add_library(topgg SHARED ${TOPGG_SOURCE_FILES})

target_include_directories(topgg PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(WIN32)
target_compile_definitions(topgg PUBLIC TOPGG_SHARED)
target_compile_definitions(topgg PUBLIC __TOPGG_BUILDING__)

if(MSVC AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/dpp.lib")
execute_process(COMMAND powershell "-NoLogo" "-NoProfile" "-File" ".\\setup_dpp.ps1" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

targeT_link_directories(topgg PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/deps")
target_link_libraries(topgg PUBLIC dpp)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Oi /Oy /Gy")
endif()
endif()

set_target_properties(topgg PROPERTIES
  OUTPUT_NAME           topgg
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON
)