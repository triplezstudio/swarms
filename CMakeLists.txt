cmake_minimum_required (VERSION 3.28)

option (ENABLE_TESTS "Enable tests" OFF)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

project (swarms LANGUAGES CXX)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set (CMAKE_VERBOSE_MAKEFILE OFF)

set (CMAKE_BUILD_RPATH_USE_ORIGIN true)

add_library (swarms-interface-library INTERFACE)
target_compile_options (swarms-interface-library INTERFACE
	-Wall -Wextra -Werror
	)

add_library (swarms-interface-test-library INTERFACE)
target_compile_options (swarms-interface-test-library INTERFACE
	--coverage
	-fprofile-update=atomic
	-O0
	-fsanitize=address
	)
target_link_options (swarms-interface-test-library INTERFACE
	--coverage
	-fsanitize=address
	)

add_subdirectory (
	${CMAKE_CURRENT_SOURCE_DIR}/src
	)

if (${ENABLE_TESTS})
	add_subdirectory (
		${CMAKE_CURRENT_SOURCE_DIR}/tests
		)
endif ()
