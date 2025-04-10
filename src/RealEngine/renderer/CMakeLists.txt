real_target_sources(RealEngine
    PUBLIC
        Allocator.hpp               
        DeletionQueue.hpp           DeletionQueue.cpp
        PipelineHotLoader.hpp       PipelineHotLoader.cpp
        VulkanFixture.hpp           VulkanFixture.cpp
        VulkanObjectBase.hpp        
    PRIVATE
        DebugMessageHandler.hpp     DebugMessageHandler.cpp
        PhysDeviceSuitability.hpp   PhysDeviceSuitability.cpp
)
