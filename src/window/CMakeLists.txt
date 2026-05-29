

add_library(window SHARED)
target_sources(window PRIVATE window.cpp)
target_include_directories(window PUBLIC include)
target_compile_definitions(window PUBLIC EXPORT_TZ_LIB)
target_link_libraries(window
        PUBLIC
        SDL2::SDL2
        SDL2::SDL2main
)

# SDL2 package:
find_package(SDL2 CONFIG REQUIRED)

# Fixing an include quirk with Apple to allow every platform to use: #include <SDL2/SDL.h>
if (APPLE)
    get_target_property(_sdl2_inc SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
    if (_sdl2_inc)
        get_filename_component(_sdl2_inc_parent "${_sdl2_inc}" DIRECTORY)
        set_property(TARGET SDL2::SDL2 APPEND PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES "${_sdl2_inc_parent}")

    endif()
endif()




