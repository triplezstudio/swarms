add_library(app SHARED)

target_include_directories(app PUBLIC include)

target_sources(app PRIVATE app.cpp text_rendering.cpp)

target_link_libraries(app PUBLIC window_sdl2_lib render_lib render_vulkan_lib window_lib text_render_lib input_sdl2_lib)

target_compile_definitions(app PRIVATE EXPORT_TZ_LIB)