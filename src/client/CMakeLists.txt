add_executable (swarms_client)

add_subdirectory(lib)

target_sources(swarms_client PRIVATE main.cc)

target_link_libraries(swarms_client PRIVATE swarms_client_lib)

