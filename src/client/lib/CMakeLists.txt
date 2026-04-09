
find_package(SDL2 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)


add_library(swarms_client_lib SHARED)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

target_sources(swarms_client_lib PRIVATE
                app.cpp
                sdl2.cpp
                opengl_renderer.cpp
)

target_include_directories(swarms_client_lib
        PUBLIC ${CMAKE_SOURCE_DIR}/src
)


target_link_libraries(swarms_client_lib
                PRIVATE
                    SDL2::SDL2 SDL2::SDL2main GLEW::GLEW
                PUBLIC
                    Eigen3::Eigen
)

if (USE_VULKAN)
    message(STATUS "Trying to find VULKAN on system")
    find_package(Vulkan REQUIRED)
    message(STATUS "Vulkan found $ENV{VULKAN_SDK}")

    target_sources(swarms_client_lib
            PUBLIC
            FILE_SET CXX_MODULES
            FILES
            vulkan.cppm

            PRIVATE
            app_sdl2_vulkan.cpp
            vulkan.cpp
            vulkan_storage.cpp

    )

    target_link_libraries(swarms_client_lib PRIVATE
            Vulkan::Vulkan)

    # --------------------------------------------------------------------------
    # Shader compilation (GLSL -> SPIR-V)
    # --------------------------------------------------------------------------

    find_program(SLANGC slang HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin")
    find_program(GLSLC glslc HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin")

    if(NOT GLSLC)
        # Try the PATH as fallback
        find_program(GLSLC glslc)
    endif()

    if(NOT SLANGC)
        # Try the PATH as fallback
        find_program(SLANGC slangc)
    endif()

    if(NOT SLANGC)
        message(FATAL_ERROR "slangc not found. Install the Vulkan SDK or add slangc to PATH.")
    else()
        message(STATUS "slangc found!")
    endif()

    if(NOT GLSLC)
        message(FATAL_ERROR "glslc not found. Install the Vulkan SDK or add glslc to PATH.")
    else()
        message(STATUS "glslc found!")
    endif()

    set(SHADER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/client/lib/shader_src)
    set(SHADER_BINARY_DIR ${CMAKE_BINARY_DIR}/bin/shader_binaries)

    message(STATUS "--- Shader Debug Info ---")
    message(STATUS "Source Dir: ${SHADER_SOURCE_DIR}")
    message(STATUS "Binary Dir: ${SHADER_BINARY_DIR}")
    message(STATUS "Compiler GLSL:   ${GLSLC}")
    message(STATUS "Compiler SLANG:   ${SLANGC}")

    file(GLOB SHADER_SOURCES CONFIGURE_DEPENDS
            ${SHADER_SOURCE_DIR}/*.vert
            ${SHADER_SOURCE_DIR}/*.frag
            ${SHADER_SOURCE_DIR}/*.comp
            ${SHADER_SOURCE_DIR}/*.slang

    )

    file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

    foreach(SHADER ${SHADER_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        set(SHADER_OUTPUT ${SHADER_BINARY_DIR}/${SHADER_NAME}.spv)

        add_custom_command(
                OUTPUT ${SHADER_OUTPUT}
                COMMAND ${SLANGC} ${SHADER} -o ${SHADER_OUTPUT}
                DEPENDS ${SHADER}
                COMMENT "Compiling shader: ${SHADER_NAME}"
        )

        list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
    endforeach()

    add_custom_target(shaders ALL DEPENDS ${SHADER_OUTPUTS})
    add_dependencies(swarms_client shaders)

else()
    message(STATUS "Not using Vulkan!")
endif()


target_compile_definitions(swarms_client_lib PRIVATE EXPORT_SWARMS_LIB)

