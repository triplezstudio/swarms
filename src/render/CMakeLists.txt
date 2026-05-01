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

target_link_libraries(render_lib INTERFACE window_lib Eigen3::Eigen stb)

# --------------------------------------------------------------------------
# Assets copying
# --------------------------------------------------------------------------
set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/render/assets)
set(ASSETS_BINARY_DIR ${CMAKE_BINARY_DIR}/bin/assets)

file(GLOB_RECURSE ASSETS_FILES RELATIVE ${ASSETS_SOURCE_DIR}
    "${ASSETS_SOURCE_DIR}/*"
)

set(ASSET_OUTPUT_FILES)

foreach(ASSET_PATH ${ASSETS_FILES})
    set(SRC ${ASSETS_SOURCE_DIR}/${ASSET_PATH})
    set(DST ${ASSETS_BINARY_DIR}/${ASSET_PATH})

    add_custom_command(
            OUTPUT ${DST}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC} ${DST}
            DEPENDS ${SRC}
            COMMENT "Copying ${ASSET_PATH}"
            VERBATIM
    )
    list(APPEND ASSET_OUTPUT_FILES ${DST})
endforeach ()

add_custom_target(copy_assets ALL DEPENDS ${ASSET_OUTPUT_FILES})

add_subdirectory(vulkan)
add_subdirectory(text)
