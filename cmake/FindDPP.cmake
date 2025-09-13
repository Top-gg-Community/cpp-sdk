get_filename_component(CMAKE_CURRENT_LIST_DIRECTORY "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(WIN32 AND NOT EXISTS ${CMAKE_CURRENT_LIST_DIRECTORY}/../deps/dpp.lib)
string(TOLOWER ${CMAKE_BUILD_TYPE} INSTALL_DPP_BUILD_TYPE)
execute_process(COMMAND powershell "-NoLogo" "-NoProfile" "-File" "${CMAKE_CURRENT_LIST_DIRECTORY}/../install_dpp_msvc.ps1" ${INSTALL_DPP_BUILD_TYPE} WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIRECTORY}/..")
endif()

if(APPLE)
find_path(DPP_INCLUDE_DIR NAMES dpp/dpp.h HINTS "/opt/homebrew/include")
find_library(DPP_LIBRARIES NAMES dpp "libdpp.a" HINTS "/opt/homebrew/lib")
else()
find_path(DPP_INCLUDE_DIR NAMES dpp/dpp.h HINTS ${CMAKE_CURRENT_LIST_DIRECTORY}/../include)
find_library(DPP_LIBRARIES NAMES dpp "libdpp.a" HINTS ${CMAKE_CURRENT_LIST_DIRECTORY}/../deps)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DPP DEFAULT_MSG DPP_LIBRARIES DPP_INCLUDE_DIR)