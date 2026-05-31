add_library(ui SHARED)

target_include_directories(ui PUBLIC include)

target_sources(ui PRIVATE ui.cpp)

target_link_libraries(ui PUBLIC render_lib render_vulkan_lib text_render_lib helpers imm_commands input_sdl2_lib)

target_compile_definitions(ui PRIVATE EXPORT_TZ_1LIB)