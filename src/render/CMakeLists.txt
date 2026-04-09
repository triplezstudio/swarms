find_package(Eigen3 CONFIG REQUIRED)

add_library(render_lib INTERFACE)

target_include_directories(render_lib INTERFACE include)

target_link_libraries(render_lib INTERFACE window_lib)

add_subdirectory(opengl)
add_subdirectory(vulkan)
