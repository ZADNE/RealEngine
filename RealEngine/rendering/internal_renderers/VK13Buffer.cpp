/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13Buffer.hpp>

namespace RE {

using enum BufferType;
using enum BufferStorage;
using enum BufferUsageFlags;
using enum BufferMapUsageFlags;

BufferID VK13Buffer::constructImmutable(size_t sizeInBytes, BufferUsageFlags flags, const void* data) const {
    return BufferID{0, 0};
}

BufferID VK13Buffer::constructMutable(size_t sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const {
    return BufferID{0, 0};
}

void VK13Buffer::destruct(BufferID& bf) const {

}

void VK13Buffer::bind(const BufferID& bf, BufferType bindType) const {

}

void VK13Buffer::bindIndexed(const BufferID& bf, const BufferTypedIndex& index) const {

}

void VK13Buffer::overwrite(const BufferID& bf, size_t offsetInBytes, size_t countBytes, const void* data) const {

}

void VK13Buffer::redefine(BufferID& bf, size_t sizeInBytes, const void* data) const {

}

void VK13Buffer::invalidate(const BufferID& bf) const {

}

void VK13Buffer::invalidate(const BufferID& bf, size_t lengthInBytes) const {

}

void* VK13Buffer::map(const BufferID& bf, size_t offsetInBytes, size_t lengthInBytes, BufferMapUsageFlags mappingUsage) const {
    return nullptr;
}

void VK13Buffer::flushMapped(const BufferID& bf, size_t offsetInBytes, size_t lengthInBytes) const {

}

bool VK13Buffer::unmap(const BufferID& bf) const {
    return false;
}

}