
find_package(SDL2 CONFIG REQUIRED)

add_library(window_sdl2_lib SHARED)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

target_sources(window_sdl2_lib PRIVATE
                sdl2.cpp   
)

target_include_directories(window_sdl2_lib
        PRIVATE ${CMAKE_SOURCE_DIR}/.
        PUBLIC include
)


target_link_libraries(window_sdl2_lib
                PRIVATE
                    SDL2::SDL2 SDL2::SDL2main window_lib
)

target_compile_definitions(window_sdl2_lib PRIVATE EXPORT_WINDOW_SDL2_LIB)