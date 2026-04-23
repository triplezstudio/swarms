add_library(app SHARED)

target_include_directories(app PUBLIC include)

target_sources(app PRIVATE app.cpp)

target_link_libraries(app PRIVATE window_sdl2_lib render_lib render_vulkan_lib window_lib)

target_compile_definitions(app PRIVATE EXPORT_TZ_LIB)