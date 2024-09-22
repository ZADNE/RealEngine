real_target_sources(RealEngine
    PUBLIC
        Allocator.hpp               
        DeletionQueue.hpp           DeletionQueue.cpp
        VulkanFixture.hpp           VulkanFixture.cpp
        VulkanObjectBase.hpp        
    PRIVATE
        DebugMessageHandler.hpp     DebugMessageHandler.cpp
        PhysDeviceSuitability.hpp   PhysDeviceSuitability.cpp
)
