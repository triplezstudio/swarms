add_library(helpers SHARED
        scene.cpp
        )

target_include_directories(helpers PUBLIC include)

target_sources(helpers PRIVATE text_rendering.cpp scene.cpp)

target_link_libraries(helpers PUBLIC render_lib render_vulkan_lib)

target_compile_definitions(helpers PRIVATE EXPORT_TZ_1LIB)