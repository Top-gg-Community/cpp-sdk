# FindDPP.cmake

message(STATUS "DPP_ROOT is set to: ${DPP_ROOT}")

# Find DPP library
find_library(DPP_LIBRARY
    NAMES dpp dpp.lib libdpp
    PATHS 
        "${DPP_ROOT}/lib"
        "${DPP_ROOT}/lib/Release"
        "${DPP_ROOT}/lib/Debug"
    NO_DEFAULT_PATH
)

message(STATUS "DPP_LIBRARY found at: ${DPP_LIBRARY}")

# Find DPP headers
find_path(DPP_INCLUDE_DIR
    NAMES dpp/dpp.h
    PATHS "${DPP_ROOT}/include"
    NO_DEFAULT_PATH
)

message(STATUS "DPP_INCLUDE_DIR found at: ${DPP_INCLUDE_DIR}")

get_filename_component(DPP_LIBRARY_DIR ${DPP_LIBRARY} DIRECTORY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DPP
    REQUIRED_VARS DPP_LIBRARY DPP_INCLUDE_DIR
)

if(DPP_FOUND AND NOT TARGET DPP::DPP)
    add_library(DPP::DPP UNKNOWN IMPORTED)
    set_target_properties(DPP::DPP PROPERTIES
        IMPORTED_LOCATION "${DPP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${DPP_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(DPP_INCLUDE_DIR DPP_LIBRARY)