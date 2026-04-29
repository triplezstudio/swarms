
add_executable (swarms_server)

add_subdirectory (
	${CMAKE_CURRENT_SOURCE_DIR}/lib
	)

target_sources (swarms_server PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cc
	)

target_include_directories (swarms_server PUBLIC
	)

target_link_libraries (swarms_server PUBLIC
	swarms-interface-library
	swarms_server_lib
	log_lib
	runtime_lib
	)

if (${ENABLE_TESTS})
	target_link_libraries (swarms_server PUBLIC
		swarms-interface-test-library
		)
endif ()
