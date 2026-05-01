add_library(text_render_lib SHARED)

target_include_directories(text_render_lib PUBLIC include)

target_sources(text_render_lib PUBLIC text_render.cpp)

target_link_libraries(text_render_lib PRIVATE
        stb
        render_vulkan_lib
)

target_compile_definitions(render_vulkan_lib PRIVATE EXPORT_TZ_LIB)