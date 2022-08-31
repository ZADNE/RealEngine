/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec2.hpp>

#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>

namespace RE {

enum class VertexComponentCount : int {
    ONE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,

    X = ONE,
    XY = TWO,
    XYZ = THREE,
    XYZW = FOUR,

    R = ONE,
    RG = TWO,
    RGB = THREE,
    RGBA = FOUR
};

enum class VertexComponentType : unsigned int {
    BYTE,
    SHORT,
    INT,
    UNSIGNED_BYTE,
    UNSIGNED_SHORT,
    UNSIGNED_INT,
    FIXED,
    HALF_FLOAT,
    FLOAT,
    DOUBLE
};

enum class Primitive : unsigned int {
    POINTS,
    LINE_STRIP,
    LINE_LOOP,
    LINES,
    LINE_STRIP_ADJACENCY,
    LINES_ADJACENCY,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
    TRIANGLES,
    TRIANGLE_STRIP_ADJACENCY,
    TRIANGLES_ADJACENCY,
    PATCHES
};

enum class IndexType : unsigned int {
    UNSIGNED_BYTE,
    UNSIGNED_SHORT,
    UNSIGNED_INT
};

/**
 * @brief Contains all members of VertexArray
 * @note For internal usage in RealEngine.
*/
class VertexArrayInternals {
    friend class GL46_VertexArray;
public:

    ~VertexArrayInternals() = default;

    VertexArrayInternals(const VertexArrayInternals&) = delete;
    VertexArrayInternals(VertexArrayInternals&& other) noexcept :
        m_id(other.m_id) {
        other.m_id = 0;
    }

    VertexArrayInternals& operator=(const VertexArrayInternals&) = delete;
    VertexArrayInternals& operator=(VertexArrayInternals&& other) noexcept {
        std::swap(m_id, other.m_id);
        return *this;
    }

private:

    VertexArrayInternals(unsigned int id) :
        m_id(id) {}

    unsigned int m_id = 0;      /**< Internal identifier */
};

/**
* @brief Is a renderer-agnostic interface to VertexArray's implementation.
*
* This is used internally by VertexArray.
*
* @see RE::VertexArray
*/
class IVertexArray {
public:

    virtual VertexArrayInternals construct() const = 0;
    virtual void destruct(VertexArrayInternals& va) const = 0;

    virtual void setAttribute(VertexArrayInternals& va, unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize = true) const = 0;
    virtual void unsetAttribute(VertexArrayInternals& va, unsigned int attribute) const = 0;

    virtual void setBindingPoint(VertexArrayInternals& va, unsigned int bindingPoint, const BufferInternals& bf, int offset, int stride) const = 0;
    virtual void unsetBindingPoint(VertexArrayInternals& va, unsigned int bindingPoint) const = 0;

    virtual void connectAttributeToBindingPoint(VertexArrayInternals& va, unsigned int attribute, unsigned int bindingPoint) const = 0;

    virtual void bind(const VertexArrayInternals& va) const = 0;
    virtual void unbind(const VertexArrayInternals& va) const = 0;

    virtual void renderArrays(const VertexArrayInternals& va, Primitive prim, int first, size_t count) const = 0;
    virtual void renderArrays(const VertexArrayInternals& va, Primitive prim, int first, size_t count, int instancecount) const = 0;

    virtual void renderElements(const VertexArrayInternals& va, Primitive prim, size_t count, IndexType type, const void* offset) const = 0;
    virtual void renderElements(const VertexArrayInternals& va, Primitive prim, size_t count, IndexType type, const void* offset, int instancecount) const = 0;

    virtual void renderElementsBaseVertex(const VertexArrayInternals& va, Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const = 0;
    virtual void renderElementsBaseVertex(const VertexArrayInternals& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const = 0;

};

}