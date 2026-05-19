add_library(helpers SHARED
        include/render_helpers.hh
        include/scene_helper.hh
        scene.cpp
        include/bindless_texture_registry.hh)

target_include_directories(helpers PUBLIC include)

target_sources(helpers PRIVATE text_rendering.cpp scene.cpp)

target_link_libraries(helpers PUBLIC render_lib render_vulkan_lib text_render_lib)

target_compile_definitions(helpers PRIVATE EXPORT_TZ_1LIB)