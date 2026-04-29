
add_library(window_lib INTERFACE)

target_include_directories(window_lib INTERFACE include)

add_subdirectory(sdl2)

target_compile_definitions(window_lib INTERFACE EXPORT_SWARMS_LIB)

if(USE_VULKAN)
    target_compile_definitions(window_lib INTERFACE USE_VULKAN)
endif()


