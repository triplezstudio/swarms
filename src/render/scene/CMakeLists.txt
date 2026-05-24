add_library(scene SHARED)

target_include_directories(scene PUBLIC include)

target_sources(scene PRIVATE scene.cpp)

target_link_libraries(scene PUBLIC render_lib render_vulkan_lib text_render_lib helpers)

target_compile_definitions(scene PRIVATE EXPORT_TZ_1LIB)