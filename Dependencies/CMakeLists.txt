include (ExternalProject)

set (Detours_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/Detours)
ExternalProject_Add(
    DetoursEP
    SOURCE_SUBDIR src
    SOURCE_DIR "${Detours_SOURCE_DIR}"
    CONFIGURE_COMMAND ${CMAKE_CURRENT_LIST_DIR}/configure_detours.bat "${Detours_SOURCE_DIR}" "<BINARY_DIR>" "${CMAKE_CURRENT_LIST_DIR}"
    BUILD_COMMAND cd src && nmake all
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "<BINARY_DIR>/lib.X64/detours.lib"
    BUILD_BYPRODUCTS "<BINARY_DIR>/include/detours.h"
)

add_library (Detours IMPORTED STATIC)
add_dependencies (Detours DetoursEP)
set_target_properties (Detours PROPERTIES
    IMPORTED_LOCATION ${Detours_SOURCE_DIR}/lib.X64/detours.lib
    INTERFACE_INCLUDE_DIRECTORIES ${Detours_SOURCE_DIR}/include
)