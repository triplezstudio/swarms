add_executable (swarms_client)

target_sources(swarms_client PRIVATE main.cc)

target_link_libraries(swarms_client PRIVATE window_lib window_sdl2_lib render_lib render_opengl_lib render_vulkan_lib)

if(USE_VULKAN)
    target_compile_definitions(swarms_client PRIVATE USE_VULKAN)
endif()

