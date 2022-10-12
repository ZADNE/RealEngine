add_subdirectory(batches)
add_subdirectory(buffers)
add_subdirectory(cameras)
add_subdirectory(internal_interfaces)
add_subdirectory(internal_renderers)
add_subdirectory(output)
add_subdirectory(textures)
add_subdirectory(vertices)

target_sources(RealEngine PRIVATE
    Capabilities.hpp            Capabilities.cpp
    Ordering.hpp                Ordering.cpp
    Renderer.hpp                
)
