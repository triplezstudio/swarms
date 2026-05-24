add_library(helpers SHARED)

target_include_directories(helpers PUBLIC include)

target_sources(helpers PRIVATE text_rendering.cpp shapes.cpp)

target_link_libraries(helpers PRIVATE render_lib render_vulkan_lib text_render_lib)

target_compile_definitions(helpers PRIVATE EXPORT_TZ_1LIB)