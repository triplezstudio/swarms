find_package(Eigen3 CONFIG REQUIRED)
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG 31c1ad37456438565541f4919958214b6e762fb4
)
FetchContent_MakeAvailable(stb)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

add_library(render_lib INTERFACE)

#target_include_directories(render_lib INTERFACE include)

target_link_libraries(render_lib INTERFACE window_lib Eigen3::Eigen stb)

# --------------------------------------------------------------------------
# Assets copying
# --------------------------------------------------------------------------
set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/render/assets)
set(ASSETS_BINARY_DIR ${CMAKE_BINARY_DIR}/bin/assets)

file(GLOB_RECURSE ASSETS_FILES CONFIGURE_DEPENDS
    ${ASSETS_SOURCE_DIR}/*
)

add_custom_command(
    OUTPUT ${ASSETS_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR} ${ASSETS_BINARY_DIR}
    DEPENDS ${ASSETS_FILES}
    COMMENT "Copying assets to build directory"
    VERBATIM
)

add_custom_target(copy_assets ALL DEPENDS ${ASSETS_BINARY_DIR})

add_subdirectory(vulkan)
