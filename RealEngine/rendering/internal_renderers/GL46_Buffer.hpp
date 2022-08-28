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
class GL46_Buffer : public IBuffer {
public:

    void constructImmutable(Buffer& bf, size_t sizeInBytes, BufferUsageFlags flags, const void* data) const override;
    void constructMutable(Buffer& bf, size_t sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const override;
    void destruct(Buffer& bf) const override;

    void bind(const Buffer& bf, BufferType bindType) const override;
    void bindIndexed(const Buffer& bf, const BufferTypedIndex& index) const override;

    void overwrite(const Buffer& bf, size_t offsetInBytes, size_t countBytes, const void* data) const override;

    void redefine(Buffer& bf, size_t sizeInBytes, const void* data) const override;

    void invalidate(const Buffer& bf) const override;
    void invalidate(const Buffer& bf, size_t lengthInBytes) const override;

    void* map(const Buffer& bf, size_t offsetInBytes, size_t lengthInBytes, BufferMapUsageFlags mappingUsage) const override;
    void flushMapped(const Buffer& bf, size_t offsetInBytes, size_t lengthInBytes) const override;
    bool unmap(const Buffer& bf) const override;

};

}