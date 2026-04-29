
add_library (simulation_lib SHARED "")

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

target_include_directories (simulation_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

target_sources (simulation_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/RandomInitializer.cc
	)

target_link_libraries (simulation_lib
	runtime_lib
	core_lib
	)
