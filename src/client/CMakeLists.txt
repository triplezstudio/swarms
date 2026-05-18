add_executable (swarms_client
        client_impl.cpp)

target_sources(swarms_client PRIVATE main.cc client_impl.cpp)

target_link_libraries(swarms_client PRIVATE window_lib window_sdl2_lib render_lib render_vulkan_lib app
client_common window_helpers)


add_dependencies(swarms_client copy_assets)

