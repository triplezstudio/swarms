

add_library(window SHARED)
target_sources(window PRIVATE window.cpp)
target_include_directories(window PUBLIC include)
target_compile_definitions(window PUBLIC EXPORT_TZ_LIB)
target_link_libraries(window
        PUBLIC
        SDL3::SDL3
)

# SDL3 package:
find_package(SDL3 CONFIG REQUIRED)

# Fixing an include quirk with Apple to allow every platform to use: #include <SDL3/SDL.h>
if (APPLE)
    get_target_property(_sdl3_inc SDL3::SDL3 INTERFACE_INCLUDE_DIRECTORIES)
    if (_sdl3_inc)
        get_filename_component(_sdl3_inc_parent "${_sdl3_inc}" DIRECTORY)
        set_property(TARGET SDL3::SDL3 APPEND PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES "${_sdl3_inc_parent}")

    endif()
endif()




