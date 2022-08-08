/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/buffers/Buffer.hpp>

#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>

namespace RE {

IBuffer* Buffer::s_impl = nullptr;

Buffer::Buffer(GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data/* = nullptr*/) {
	s_impl->constructImmutable(*this, sizeInBytes, flags, data);
}


Buffer::Buffer(GLsizeiptr sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data/* = nullptr*/) {
	s_impl->constructMutable(*this, sizeInBytes, accessFreq, accessNature, data);
}

Buffer::~Buffer() {
	s_impl->destruct(*this);
}

Buffer::Buffer(Buffer&& other) noexcept :
	m_ID(other.m_ID),
	m_sizeInBytes(other.m_sizeInBytes),
	m_access(other.m_access)
#ifndef _DEBUG
{
#else
	, m_storage(other.m_storage) {
#endif // _DEBUG
	other.m_ID = 0;
}

Buffer& Buffer::operator=(Buffer && other) noexcept {
	std::swap(m_ID, other.m_ID);
	m_sizeInBytes = other.m_sizeInBytes;
	m_access = other.m_access;
#ifdef _DEBUG
	m_storage = other.m_storage;
#endif // _DEBUG
	return *this;
}

void Buffer::bind(BufferType bindType) {
	s_impl->bind(*this, bindType);
}

void Buffer::bindIndexed(const BufferTypedIndex& index) {
	s_impl->bindIndexed(*this, index);
}

void Buffer::overwrite(GLintptr offsetInBytes, GLsizeiptr countBytes, const void* data) {
	s_impl->overwrite(*this, offsetInBytes, countBytes, data);
}

void Buffer::redefine(GLsizeiptr sizeInBytes, const void* data) {
	s_impl->redefine(*this, sizeInBytes, data);
}

void Buffer::invalidate() {
	s_impl->invalidate(*this);
}

void Buffer::invalidate(GLsizeiptr lengthInBytes) {
	s_impl->invalidate(*this, lengthInBytes);
}

void Buffer::flushMapped(GLintptr offsetInBytes, GLsizeiptr lengthInBytes) {
	s_impl->flushMapped(*this, offsetInBytes, lengthInBytes);
}

bool Buffer::unmap() {
	return s_impl->unmap(*this);
}

}