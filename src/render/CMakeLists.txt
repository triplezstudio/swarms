


add_library(render_lib SHARED
        drawing.cpp)

target_link_libraries(render_lib PRIVATE window_lib window_sdl2_lib
        stb client_common render_vulkan_lib text_render_lib Eigen3::Eigen)
target_include_directories(render_lib PUBLIC include)
target_compile_definitions(render_lib PUBLIC EXPORT_TZ_LIB)

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
