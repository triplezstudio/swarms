add_library(app SHARED)

target_include_directories(app PUBLIC include)

target_sources(app PRIVATE app.cpp)

target_link_libraries(app PUBLIC render_lib render_vulkan_lib window text_render_lib input_sdl_lib helpers scene imm_commands ui)

target_compile_definitions(app PRIVATE EXPORT_TZ_LIB)