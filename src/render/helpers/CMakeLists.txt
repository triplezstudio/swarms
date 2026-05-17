add_library(render_helpers SHARED)


target_sources(render_helpers PUBLIC text_render_helper.cpp)

target_include_directories(render_helpers PUBLIC include)


target_link_libraries(render_helpers PRIVATE
        render_vulkan_lib scene text_render_lib
)

target_compile_definitions(render_helpers PRIVATE EXPORT_TZ_LIB)