real_target_sources(RealEngine
    PUBLIC
        Allocator.hpp               
        DeletionQueue.hpp           DeletionQueue.cpp
        HotReloadInitInfo.hpp       
        ObjectUsingVulkan.hpp       
        PipelineHotLoader.hpp       PipelineHotLoader.cpp
        VulkanFixture.hpp           VulkanFixture.cpp
    PRIVATE
        DebugMessageHandler.hpp     DebugMessageHandler.cpp
        PhysDeviceSuitability.hpp   PhysDeviceSuitability.cpp
)
