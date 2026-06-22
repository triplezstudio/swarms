add_library(input_sdl_lib SHARED)

target_sources(input_sdl_lib PRIVATE
        sdl_input.cpp)

target_include_directories(input_sdl_lib PUBLIC include)

target_link_libraries(input_sdl_lib PUBLIC window)

target_compile_definitions(input_sdl_lib PUBLIC EXPORT_TZ_LIB)