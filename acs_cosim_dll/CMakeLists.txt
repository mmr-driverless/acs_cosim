add_library (acs_cosim_dll SHARED src/dllmain.cpp src/Parameters.cpp src/winapi_helpers.cpp)
target_link_libraries (acs_cosim_dll Detours acs_cosim_interface_server)
target_compile_definitions (acs_cosim_dll PRIVATE WIN32_LEAN_AND_MEAN _UNICODE UNICODE)
target_compile_features (acs_cosim_dll PUBLIC cxx_std_20)
install (TARGETS acs_cosim_dll
	DESTINATION assettocorsa
)