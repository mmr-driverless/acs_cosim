add_executable (acs_cosim_launcher src/Launcher.cpp)
target_link_libraries (acs_cosim_launcher Detours)
target_compile_definitions (acs_cosim_launcher PRIVATE WIN32_LEAN_AND_MEAN _UNICODE UNICODE)
target_compile_features (acs_cosim_launcher PUBLIC cxx_std_20)
install (TARGETS acs_cosim_launcher
	DESTINATION assettocorsa
)