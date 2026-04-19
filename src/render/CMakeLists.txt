find_package(Eigen3 CONFIG REQUIRED)

add_library(render_lib STATIC)

target_include_directories(render_lib PUBLIC include)
target_sources(render_lib PUBLIC renderer.cpp)

target_link_libraries(render_lib PUBLIC window_lib Eigen3::Eigen)

add_subdirectory(opengl)
add_subdirectory(vulkan)
