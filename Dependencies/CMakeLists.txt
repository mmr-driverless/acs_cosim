add_subdirectory (Detours EXCLUDE_FROM_ALL)

option (BUILD_SERVER "" ON)
option (BUILD_CLIENT "" OFF)
option (BUILD_TESTS "" OFF)
add_subdirectory (acs_cosim_interface EXCLUDE_FROM_ALL)