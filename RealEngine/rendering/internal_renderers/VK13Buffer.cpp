/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13Buffer.hpp>

namespace RE {

BufferID VK13Buffer::construct(size_t sizeInBytes, vk::BufferUsageFlags flags, const void* data) const {
    return BufferID::VK13{.buffer = nullptr};
}

void VK13Buffer::destruct(BufferID& bf) const {

}

}