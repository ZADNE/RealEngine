real_target_sources(RealEngine
    PUBLIC
        Allocator.hpp               
        DeletionQueue.hpp           DeletionQueue.cpp
        ObjectUsingVulkan.hpp       
        PipelineHotLoader.hpp       PipelineHotLoader.cpp
        VulkanRenderer.hpp          VulkanRenderer.cpp
    PRIVATE
        DebugMessageHandler.hpp     DebugMessageHandler.cpp
        PhysDeviceSuitability.hpp   PhysDeviceSuitability.cpp
)
