/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/renderers/IBuffer.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Buffer
*
* Do not use this directly - use Buffer class instead.
*/
class GL46_Buffer : public IBuffer {
public:

	void constructImmutable(Buffer& bf, GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data) const override;
	void constructMutable(Buffer& bf, GLsizeiptr sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const override;
	void destruct(Buffer& bf) const override;

	void bind(Buffer& bf, BufferType bindType) const override;
	void bindIndexed(Buffer& bf, const BufferTypedIndex& index) const override;

	void overwrite(Buffer& bf, GLintptr offsetInBytes, GLsizeiptr countBytes, const void* data) const override;

	void redefine(Buffer& bf, GLsizeiptr sizeInBytes, const void* data) const override;

	void invalidate(Buffer& bf) const override;
	void invalidate(Buffer& bf, GLsizeiptr lengthInBytes) const override;

	void* map(Buffer& bf, GLintptr offsetInBytes, GLsizeiptr lengthInBytes, BufferMapUsageFlags mappingUsage) const override;
	void flushMapped(Buffer& bf, GLintptr offsetInBytes, GLsizeiptr lengthInBytes) const override;
	bool unmap(Buffer& bf) const override;

protected:
	using enum BufferType;
	using enum BufferStorage;
	using enum BufferAccessFrequency;
	using enum BufferAccessNature;
	using enum BufferUsageFlags;
};

}