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

	void constructImmutable(Buffer& bf, int sizeInBytes, BufferUsageFlags flags, const void* data) const override;
	void constructMutable(Buffer& bf, int sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const override;
	void destruct(Buffer& bf) const override;

	void bind(Buffer& bf, BufferType bindType) const override;
	void bindIndexed(Buffer& bf, const BufferTypedIndex& index) const override;

	void overwrite(Buffer& bf, int offsetInBytes, int countBytes, const void* data) const override;

	void redefine(Buffer& bf, int sizeInBytes, const void* data) const override;

	void invalidate(Buffer& bf) const override;
	void invalidate(Buffer& bf, int lengthInBytes) const override;

	void* map(Buffer& bf, int offsetInBytes, int lengthInBytes, BufferMapUsageFlags mappingUsage) const override;
	void flushMapped(Buffer& bf, int offsetInBytes, int lengthInBytes) const override;
	bool unmap(Buffer& bf) const override;

};

}