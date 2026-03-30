
target_include_directories (core_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

target_sources (core_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/AxisAlignedBoundingBox.cc
	${CMAKE_CURRENT_SOURCE_DIR}/CircleBox.cc
	)
