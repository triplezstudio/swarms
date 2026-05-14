add_library(render_helpers SHARED)

target_include_directories(render_helpers PUBLIC include)

target_sources(render_helpers PUBLIC text_render_helper.cpp shape_render_helper.cpp)

target_link_libraries(render_helpers PRIVATE
        render_vulkan_lib
)

target_compile_definitions(render_helpers PRIVATE EXPORT_TZ_LIB)