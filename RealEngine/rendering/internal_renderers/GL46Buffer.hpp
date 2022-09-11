/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Buffer
*
* Do not use this directly - use Buffer class instead.
*/
class GL46Buffer final : public IBuffer {
public:

    BufferID constructImmutable(size_t sizeInBytes, BufferUsageFlags flags, const void* data) const override;
    BufferID constructMutable(size_t sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const override;
    void destruct(BufferID& bf) const override;

    void bind(const BufferID& bf, BufferType bindType) const override;
    void bindIndexed(const BufferID& bf, const BufferTypedIndex& index) const override;

    void overwrite(const BufferID& bf, size_t offsetInBytes, size_t countBytes, const void* data) const override;

    void redefine(BufferID& bf, size_t sizeInBytes, const void* data) const override;

    void invalidate(const BufferID& bf) const override;
    void invalidate(const BufferID& bf, size_t lengthInBytes) const override;

    void* map(const BufferID& bf, size_t offsetInBytes, size_t lengthInBytes, BufferMapUsageFlags mappingUsage) const override;
    void flushMapped(const BufferID& bf, size_t offsetInBytes, size_t lengthInBytes) const override;
    bool unmap(const BufferID& bf) const override;
};

}