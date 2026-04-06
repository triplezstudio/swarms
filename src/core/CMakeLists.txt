
add_library (core_lib SHARED "")

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

target_include_directories (core_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/agents
	)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/components
	)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/environment
	)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/influences
	)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/perceptions
	)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/physics
	)

<<<<<<< Updated upstream
=======
add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/random
	)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/runtime
	)

>>>>>>> Stashed changes
target_sources (core_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Uuid.cc
	)

target_link_libraries (core_lib
	time_lib
	)
