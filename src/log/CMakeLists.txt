
add_library (log_lib SHARED "")

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

target_include_directories (log_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

target_sources (log_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Locator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/NullLogger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/Severity.cc
	${CMAKE_CURRENT_SOURCE_DIR}/StdLogger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/StreamFormatter.cc
	)

target_link_libraries (log_lib PUBLIC
	swarms-interface-library
	)

if (${ENABLE_TESTS})
	target_link_libraries (log_lib PUBLIC
		swarms-interface-test-library
		)
endif ()
