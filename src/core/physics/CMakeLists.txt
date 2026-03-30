
target_include_directories (core_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/bbox
	)

target_sources (core_lib PRIVATE
	)
