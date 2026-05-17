add_library(scene SHARED)

target_include_directories(scene PUBLIC include)

target_sources(scene PUBLIC scene.cpp)

target_link_libraries(scene PRIVATE
        Eigen3::Eigen render_lib render_vulkan_lib)

target_compile_definitions(scene PRIVATE EXPORT_TZ_LIB)