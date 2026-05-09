add_library(input_sdl2_lib SHARED)

target_sources(input_sdl2_lib PRIVATE
        sdl2_input.cpp)

target_include_directories(input_sdl2_lib PUBLIC include)

target_link_libraries(input_sdl2_lib PUBLIC window_sdl2_lib)

target_compile_definitions(input_sdl2_lib PUBLIC EXPORT_TZ_LIB)