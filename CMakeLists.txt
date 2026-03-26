cmake_minimum_required (VERSION 3.28)

option (ENABLE_TESTS "Enable tests" OFF)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

project (swarms LANGUAGES CXX)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set (CMAKE_VERBOSE_MAKEFILE OFF)

# RPATH is largely a Linux/macOS concept; Windows ignores this safely.
set (CMAKE_BUILD_RPATH_USE_ORIGIN true)

# --- Library Interface ---
add_library (swarms-interface-library INTERFACE)
target_compile_options (swarms-interface-library INTERFACE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Werror>
)

# --- Test Library Interface ---
add_library (swarms-interface-test-library INTERFACE)

target_compile_options (swarms-interface-test-library INTERFACE
		# GCC/Clang Specific (Coverage and Sanitizers)
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
		--coverage
		-fprofile-update=atomic
		-O0
		-fsanitize=address
		>
		# MSVC Specific (Sanitizer only - Coverage is handled differently in VS)
		$<$<CXX_COMPILER_ID:MSVC>:
		/fsanitize=address
		/Od
		>
)

target_link_options (swarms-interface-test-library INTERFACE
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
		--coverage
		-fsanitize=address
		>
		# On MSVC, ASan requires linking the library explicitly or via flags
		$<$<CXX_COMPILER_ID:MSVC>:
		/fsanitize=address
		>
)

# --- Subdirectories ---
add_subdirectory (src)

if (ENABLE_TESTS)
	add_subdirectory (tests)
endif ()