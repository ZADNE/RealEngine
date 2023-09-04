add_subdirectory(batches)
add_subdirectory(buffers)
add_subdirectory(cameras)
add_subdirectory(descriptors)
add_subdirectory(output_control)
add_subdirectory(pipelines)
add_subdirectory(synchronization)
add_subdirectory(textures)

target_sources(RealEngine PRIVATE
    CommandBuffer.hpp           CommandBuffer.cpp
)
