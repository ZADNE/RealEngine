add_subdirectory(batches)
add_subdirectory(buffers)
add_subdirectory(cameras)
add_subdirectory(pipelines)
add_subdirectory(textures)

target_sources(RealEngine PRIVATE
    CommandBuffer.hpp           CommandBuffer.cpp
    DescriptorSet.hpp           DescriptorSet.cpp
    PerFrameInFlight.hpp        PerFrameInFlight.cpp
)
