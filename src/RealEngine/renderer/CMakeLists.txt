real_target_sources(RealEngine_renderer
    PUBLIC
        Allocator.hpp               
        DeletionQueue.hpp           DeletionQueue.cpp
        ObjectUsingVulkan.hpp       
        VulkanRenderer.hpp          VulkanRenderer.cpp
    PRIVATE
        DebugMessageHandler.hpp     DebugMessageHandler.cpp
        PhysDeviceSuitability.hpp   PhysDeviceSuitability.cpp
)
