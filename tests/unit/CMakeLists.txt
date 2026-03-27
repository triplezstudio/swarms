
add_executable (unitTests)

target_sources (unitTests PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cc
	)

target_include_directories (unitTests PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

add_subdirectory (
	${CMAKE_CURRENT_SOURCE_DIR}/time
	)

target_link_libraries (unitTests
	GTest::gtest
	GTest::gmock
	swarms-interface-test-library
	time_lib
	)
