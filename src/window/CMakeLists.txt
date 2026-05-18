
add_library(window_lib INTERFACE)

target_include_directories(window_lib INTERFACE include)

add_subdirectory(sdl2)
add_subdirectory(helpers)

target_compile_definitions(window_lib INTERFACE EXPORT_TZ_LIB)


