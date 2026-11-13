real_target_sources(RealEngine_utility
    PUBLIC
        Allocator.cppm              
        DeletionQueue.cppm          DeletionQueue.cpp
        ObjectUsingVulkan.cppm      
        VulkanRenderer.cppm         VulkanRenderer.cpp
    PRIVATE
        DebugMessageHandler.cppm    DebugMessageHandler.cpp
        PhysDeviceSuitability.cppm  PhysDeviceSuitability.cpp
)
