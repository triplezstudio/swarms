
target_include_directories (core_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

target_sources (core_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/AbstractComponent.cc
	${CMAKE_CURRENT_SOURCE_DIR}/AnimatComponent.cc
	${CMAKE_CURRENT_SOURCE_DIR}/ComponentType.cc
	${CMAKE_CURRENT_SOURCE_DIR}/TransformComponent.cc
	${CMAKE_CURRENT_SOURCE_DIR}/VelocityComponent.cc
	)
