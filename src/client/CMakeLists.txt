add_executable (swarms_client)

find_package(SDL2 CONFIG REQUIRED)

target_sources(swarms_client PRIVATE main.cc)

target_link_libraries(swarms_client PRIVATE SDL2::SDL2 SDL2::SDL2main)


