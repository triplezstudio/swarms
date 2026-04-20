find_package(Eigen3 CONFIG REQUIRED)

add_library(render_lib SHARED renderer.cpp)

target_include_directories(render_lib PUBLIC include)
#target_sources(render_lib PUBLIC renderer.cpp)

target_link_libraries(render_lib PUBLIC window_lib Eigen3::Eigen)

# --------------------------------------------------------------------------
# Assets copying
# --------------------------------------------------------------------------
set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/render/assets)
set(ASSETS_BINARY_DIR ${CMAKE_BINARY_DIR}/bin/assets)

file(GLOB_RECURSE ASSETS_FILES CONFIGURE_DEPENDS
    ${ASSETS_SOURCE_DIR}/*
)

add_custom_command(
    OUTPUT ${ASSETS_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR} ${ASSETS_BINARY_DIR}
    DEPENDS ${ASSETS_FILES}
    COMMENT "Copying assets to build directory"
    VERBATIM
)

add_custom_target(copy_assets ALL DEPENDS ${ASSETS_BINARY_DIR})

add_subdirectory(opengl)
add_subdirectory(vulkan)
