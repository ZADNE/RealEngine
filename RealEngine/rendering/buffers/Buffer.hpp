/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>

namespace RE {

/**
 * @brief Is a continuous block of memory stored in the GPU's memory.
 *
 * A buffer can be either mutable or immutable. Mutable buffer can be resized,
 * while immutable buffer cannot change its size upon construction.
 * The mutability is determined by the constructor that is used to construct the buffer.
*/
class Buffer {
	friend class GL46_Renderer;
	friend class GL46_Buffer;
	friend class GL46_VertexArray;
public:

	/**
	 * @brief Constructs an immutable buffer of given size, usage and data
	 * @param sizeInBytes Size in bytes of the buffer
	 * @param flags Restrict usage of the buffer
	 * @param data	If a valid pointer is provided, it is used to initialize the contents of the buffer.
	 *				If the nullptr is provided, the contents of the buffer are undefined.
	*/
	Buffer(int sizeInBytes, BufferUsageFlags flags, const void* data = nullptr);

	/**
	 * @brief Contructs an immutable buffer as a storage for given type
	 * @tparam T The type that the buffer will store (used to determine buffer's size)
	 * @param flags Restrict usage of the buffer
	 * @param data The instance of the type that the buffer will be initialized with
	*/
	template<typename T>
	Buffer(BufferUsageFlags flags, const T& data) :
		Buffer(sizeof(T), flags, &data) {}

	/**
	 * @brief Contructs an immutable buffer as an array
	 * @tparam T Element type of the array
	 * @param flags Restrict usage of the buffer
	 * @param data Array (provided as a vector) that determines the size and initial contents of the buffer
	*/
	template<typename T>
	Buffer(BufferUsageFlags flags, const std::vector<T>& data) :
		Buffer(data.size() * sizeof(T), flags, data.data()) {}

	/**
	 * @brief Constructs a mutable buffer of given initial size, access hints and initial data
	 * @param sizeInBytes Initial size of the buffer
	 * @param accessFreq Hints how often the buffer will be access by the GPU
	 * @param accessNature Hints how the buffer will be used
	 * @param data	If a valid pointer is provided, it is used to initialize the contents of the buffer.
	 *				If the nullptr is provided, the contents of the buffer are undefined.
	*/
	Buffer(int sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data = nullptr);

	/**
	 * @brief Contructs a mutable buffer of given access hints
	 *
	 * Initial size of the buffer is zero bytes.
	 * @param accessFreq Hints how often the buffer will be access by the GPU
	 * @param accessNature Hints how the buffer will be used
	*/
	Buffer(BufferAccessFrequency accessFreq, BufferAccessNature accessNature) :
		Buffer(0u, accessFreq, accessNature, nullptr) {}

	/**
	 * @brief Frees the backing memory block on the GPU and destructs the buffer.
	*/
	~Buffer();

	Buffer(const Buffer&) = delete;

	Buffer(Buffer&& other) noexcept;

	Buffer& operator=(const Buffer&) = delete;

	Buffer& operator=(Buffer&& other) noexcept;

	/**
	 * @brief Binds the buffer to a generic binding point.
	 *
	 * Each type of buffer has its own generic binding point.
	 * @param bindType The type of binding point to bind this buffer to.
	*/
	void bind(BufferType bindType);

	/**
	 * @brief Binds the buffer to an indexed binding point (as well as generic binding point)
	 *
	 * Indexed binding is only available for these types:
	 * ATOMIC_COUNTER, TRANSFORM_FEEDBACK, UNIFORM, and SHADER_STORAGE.
	 * @param index The typed index to bind this buffer to.
	*/
	void bindIndexed(const BufferTypedIndex& index);

	/**
	 * @brief Overwrites a portion of or whole buffer
	 * @param offsetInBytes Offset in bytes within the buffer to overwrite
	 * @param countBytes Number of bytes to overwrite
	 * @param data The data to overwrite with
	*/
	void overwrite(int offsetInBytes, int countBytes, const void* data);

	/**
	 * @brief Overwrites the buffer with instance of a type
	 * @tparam T The type to overwrite the buffer with
	 * @param offsetInBytes Offset in bytes within the buffer to overwrite
	 * @param data The instance to overwrite the buffer with
	*/
	template<typename T>
	void overwrite(int offsetInBytes, const T& data) {
		overwrite(offsetInBytes, sizeof(T), reinterpret_cast<const void*>(&data));
	}

	/**
	 * @brief Overwrites the buffer with an array
	 * @tparam T Element type of the array
	 * @param offsetInBytes Offset in bytes within the buffer to overwrite
	 * @param data Array (provided as a vector) to overwrite the buffer with
	*/
	template<typename T>
	void overwrite(int offsetInBytes, const std::vector<T>& data) {
		overwrite(offsetInBytes, data.size() * sizeof(T), reinterpret_cast<const void*>(data.data()));
	}

	/**
	 * @brief Ensures that the buffer is big enough to store the data and writes it
	 *
	 * Since this operation may cause resize of the buffer,
	 * it can only be applied to mutable buffers.
	 * @param sizeInBytes Size of the data required to write
	 * @param data The data to write
	*/
	void redefine(int sizeInBytes, const void* data);

	/**
	 * @brief Ensures that the buffer is big enough to store the data and writes it
	 *
	 * Since this operation may cause resize of the buffer,
	 * it can only be applied to mutable buffers.
	 * @tparam T Type to write
	 * @param data Instance to write
	*/
	template<typename T>
	void redefine(const T& data) {
		redefine(sizeof(T), reinterpret_cast<const void*>(&data));
	}

	/**
	 * @brief Ensures that the buffer is big enough to store the data and writes it
	 *
	 * Since this operation may cause resize of the buffer,
	 * it can only be applied to mutable buffers.
	 * @tparam T Element type of the array
	 * @param data Array (provided as a vector) to redefine the buffer as
	*/
	template<typename T>
	void redefine(const std::vector<T>& data) {
		redefine(data.size() * sizeof(T), reinterpret_cast<const void*>(data.data()));
	}

	/**
	 * @brief Makes the contents of the buffer undefined
	 *
	 * This effectively frees the backing block of memory of the buffer.
	*/
	void invalidate();

	/**
	 * @brief Makes a range of the buffer undefined
	 * @param lengthInBytes Length of the range at the beginning of the buffer to invalidate
	*/
	void invalidate(int lengthInBytes);

	/**
	 * @brief Maps a range of the buffer to the client's memory
	 * @tparam T Reinterpreted type of the returned pointer
	*/
	template<typename T>
	T* map(int offsetInBytes, int lengthInBytes, BufferMapUsageFlags mappingUsage) {
		return reinterpret_cast<T*>(s_impl->map(*this, offsetInBytes, lengthInBytes, mappingUsage));
	}

	/**
	 * @brief Indicates modifications to a mapped range of the buffer
	*/
	void flushMapped(int offsetInBytes, int lengthInBytes);

	/**
	 * @brief Releases the mapping of the buffer
	 * @return True if success. Buffer's contents are undefined if false is returned.
	*/
	bool unmap();

protected:

	unsigned int m_ID = 0;				/**< Internal name of the buffer */
	int m_sizeInBytes = 0;				/**< Size of the buffer */
	unsigned int m_access = 0;			/**< Access hints of the buffer; relevant only for mutable buffers */

#ifdef _DEBUG
	BufferStorage m_storage = BufferStorage::IMMUTABLE;
#endif // _DEBUG

	static IBuffer* s_impl;
};

}