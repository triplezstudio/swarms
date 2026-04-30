
target_include_directories (simulation_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

target_sources (simulation_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/RandomAgent.cc
	)
