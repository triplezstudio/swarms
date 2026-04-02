
add_library (runtime_lib SHARED "")

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

target_include_directories (runtime_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

target_sources (runtime_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/CoreException.cc
	${CMAKE_CURRENT_SOURCE_DIR}/CoreObject.cc
	${CMAKE_CURRENT_SOURCE_DIR}/SafetyNet.cc
	)

target_link_libraries (runtime_lib PUBLIC
	swarms-interface-library
	log_lib
	)

if (${ENABLE_TESTS})
	target_link_libraries (runtime_lib PUBLIC
		swarms-interface-test-library
		)
endif ()
