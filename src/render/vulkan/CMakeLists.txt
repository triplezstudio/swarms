if (USE_VULKAN)
    message(STATUS "Trying to find VULKAN on system")
    find_package(Vulkan REQUIRED)
    message(STATUS "Vulkan found $ENV{VULKAN_SDK}")

    add_library(render_vulkan_lib SHARED)
    set (CMAKE_POSITION_INDEPENDENT_CODE ON)

    target_sources(render_vulkan_lib
            PRIVATE
            vulkan.cpp
    )

    target_include_directories(render_vulkan_lib
            PUBLIC include
    )


    target_link_libraries(render_vulkan_lib PUBLIC
            Vulkan::Vulkan  Eigen3::Eigen render_lib)

    # --------------------------------------------------------------------------
    # Shader compilation (GLSL -> SPIR-V)
    # --------------------------------------------------------------------------
    find_program(SLANGC slang HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin")

    if(NOT SLANGC)
        # Try the PATH as fallback
        find_program(SLANGC slangc)
    endif()

    if(NOT SLANGC)
        message(FATAL_ERROR "slangc not found. Install the Vulkan SDK or add slangc to PATH.")
    else()
        message(STATUS "slangc found!")
    endif()

    set(SHADER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/render/shader_src)
    set(SHADER_BINARY_DIR ${CMAKE_BINARY_DIR}/bin/shader_binaries)

    message(STATUS "--- Shader Debug Info ---")
    message(STATUS "Source Dir: ${SHADER_SOURCE_DIR}")
    message(STATUS "Binary Dir: ${SHADER_BINARY_DIR}")
    message(STATUS "Compiler SLANG:   ${SLANGC}")

    file(GLOB SHADER_SOURCES CONFIGURE_DEPENDS
            ${SHADER_SOURCE_DIR}/*.slang

    )

    file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

    foreach(SHADER ${SHADER_SOURCES})
        #get_filename_component(SHADER_NAME ${SHADER} NAME)
        file(RELATIVE_PATH SHADER_REL ${SHADER_SOURCE_DIR} ${SHADER})
        string(REPLACE "/" "_" SHADER_SAFE_NAME ${SHADER_REL})
        set(SHADER_OUTPUT ${SHADER_BINARY_DIR}/${SHADER_SAFE_NAME}.spv)

        message(STATUS "SHADER: ${SHADER}")
        message(STATUS "REL: ${SHADER_REL}")
        message(STATUS "OUT: ${SHADER_OUTPUT}")

        add_custom_command(
                OUTPUT ${SHADER_OUTPUT}
                COMMAND ${SLANGC} ${SHADER}
                        -o ${SHADER_OUTPUT}
                DEPENDS ${SHADER}
                COMMENT "Compiling shader: ${SHADER_NAME}"
                VERBATIM
        )

        list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
    endforeach()

    add_custom_target(shaders ALL DEPENDS ${SHADER_OUTPUTS})
    add_dependencies(swarms_client shaders)

    target_compile_definitions(render_vulkan_lib PRIVATE EXPORT_TZ_LIB)
endif()


