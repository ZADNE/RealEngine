add_subdirectory(batches)
add_subdirectory(buffers)
add_subdirectory(cameras)
add_subdirectory(internal_interfaces)
add_subdirectory(internal_renderers)
add_subdirectory(output)
add_subdirectory(textures)
add_subdirectory(vertices)

target_sources(${PROJECT_NAME} PUBLIC
    basic_shaders.hpp           
    capabilities.hpp            capabilities.cpp
    Ordering.hpp                Ordering.cpp
)
