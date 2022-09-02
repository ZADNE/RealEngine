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
class GL46_Buffer final : public IBuffer {
public:

    BufferInternals constructImmutable(size_t sizeInBytes, BufferUsageFlags flags, const void* data) const override;
    BufferInternals constructMutable(size_t sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const override;
    void destruct(BufferInternals& bf) const override;

    void bind(const BufferInternals& bf, BufferType bindType) const override;
    void bindIndexed(const BufferInternals& bf, const BufferTypedIndex& index) const override;

    void overwrite(const BufferInternals& bf, size_t offsetInBytes, size_t countBytes, const void* data) const override;

    void redefine(BufferInternals& bf, size_t sizeInBytes, const void* data) const override;

    void invalidate(const BufferInternals& bf) const override;
    void invalidate(const BufferInternals& bf, size_t lengthInBytes) const override;

    void* map(const BufferInternals& bf, size_t offsetInBytes, size_t lengthInBytes, BufferMapUsageFlags mappingUsage) const override;
    void flushMapped(const BufferInternals& bf, size_t offsetInBytes, size_t lengthInBytes) const override;
    bool unmap(const BufferInternals& bf) const override;
};

}