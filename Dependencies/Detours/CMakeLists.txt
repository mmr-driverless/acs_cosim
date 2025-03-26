include (ExternalProject)

ExternalProject_Add(
    DetoursEP
    SOURCE_SUBDIR src
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/Detours"
    CONFIGURE_COMMAND ${CMAKE_CURRENT_LIST_DIR}/configure_detours.bat "${CMAKE_CURRENT_LIST_DIR}" "<BINARY_DIR>"
    BUILD_COMMAND cd src && nmake all
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "<BINARY_DIR>/lib.X64/detours.lib"
    BUILD_BYPRODUCTS "<BINARY_DIR>/include/detours.h"
)

ExternalProject_Get_property(DetoursEP BINARY_DIR)
file (MAKE_DIRECTORY ${BINARY_DIR}/lib.X64)
file (MAKE_DIRECTORY ${BINARY_DIR}/include)

add_library (Detours IMPORTED STATIC GLOBAL)
add_dependencies (Detours DetoursEP)
set_target_properties (Detours PROPERTIES
    IMPORTED_LOCATION ${BINARY_DIR}/lib.X64/detours.lib
    INTERFACE_INCLUDE_DIRECTORIES ${BINARY_DIR}/include
)