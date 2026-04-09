
find_package(GLEW REQUIRED)


add_library(render_opengl_lib SHARED)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

target_sources(render_opengl_lib PRIVATE
                opengl_renderer.cpp
)

target_include_directories(render_opengl_lib
        PUBLIC include
)


target_link_libraries(render_opengl_lib
                PRIVATE
                    GLEW::GLEW
                PUBLIC
                    Eigen3::Eigen render_lib
)


target_compile_definitions(render_opengl_lib PRIVATE EXPORT_RENDER_OPENGL_LIB)