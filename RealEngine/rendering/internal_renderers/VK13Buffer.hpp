/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>

namespace RE {

/**
* @brief Is Vulkan 1.3 implementation of Buffer
*
* Do not use this directly - use Buffer class instead.
*/
class VK13Buffer final : public IBuffer {
public:

    BufferID construct(size_t sizeInBytes, vk::BufferUsageFlags flags, const void* data) const override;
    void destruct(BufferID& bf) const override;
};

}