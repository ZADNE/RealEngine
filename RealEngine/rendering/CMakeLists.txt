add_subdirectory(basic_shaders)
add_subdirectory(batches)
add_subdirectory(buffers)
add_subdirectory(cameras)
add_subdirectory(textures)
add_subdirectory(vertices)

target_sources(RealEngine PRIVATE
    DescriptorSet.hpp           DescriptorSet.cpp
    PerFrameInFlight.hpp        PerFrameInFlight.cpp
    Pipeline.hpp                Pipeline.cpp
    Renderer.hpp                
)
