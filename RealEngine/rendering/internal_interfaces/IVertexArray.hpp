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
class VertexArrayID {
    friend class GL46VertexArray;
public:

    ~VertexArrayID() = default;

    VertexArrayID(const VertexArrayID&) = delete;
    VertexArrayID(VertexArrayID&& other) noexcept :
        m_id(other.m_id) {
        other.m_id = 0;
    }

    VertexArrayID& operator=(const VertexArrayID&) = delete;
    VertexArrayID& operator=(VertexArrayID&& other) noexcept {
        std::swap(m_id, other.m_id);
        return *this;
    }

private:

    VertexArrayID(unsigned int id) :
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

    virtual VertexArrayID construct() const = 0;
    virtual void destruct(VertexArrayID& va) const = 0;

    virtual void setAttribute(VertexArrayID& va, unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize = true) const = 0;
    virtual void unsetAttribute(VertexArrayID& va, unsigned int attribute) const = 0;

    virtual void setBindingPoint(VertexArrayID& va, unsigned int bindingPoint, const BufferID& bf, int offset, int stride) const = 0;
    virtual void unsetBindingPoint(VertexArrayID& va, unsigned int bindingPoint) const = 0;

    virtual void connectAttributeToBindingPoint(VertexArrayID& va, unsigned int attribute, unsigned int bindingPoint) const = 0;

    virtual void bind(const VertexArrayID& va) const = 0;
    virtual void unbind(const VertexArrayID& va) const = 0;

    virtual void renderArrays(const VertexArrayID& va, Primitive prim, int first, size_t count) const = 0;
    virtual void renderArrays(const VertexArrayID& va, Primitive prim, int first, size_t count, int instancecount) const = 0;

    virtual void renderElements(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* offset) const = 0;
    virtual void renderElements(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* offset, int instancecount) const = 0;

    virtual void renderElementsBaseVertex(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const = 0;
    virtual void renderElementsBaseVertex(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const = 0;
};

}