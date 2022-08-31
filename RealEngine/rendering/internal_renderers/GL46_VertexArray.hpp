/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IVertexArray.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of VertexArray class.
*
* Do not use this directly - use VertexArray instead.
*/
class GL46_VertexArray final : public IVertexArray {
public:

    VertexArrayInternals construct() const override;
    void destruct(VertexArrayInternals& va) const override;

    void setAttribute(VertexArrayInternals& va, unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize = true) const override;
    void unsetAttribute(VertexArrayInternals& va, unsigned int attribute) const override;

    void setBindingPoint(VertexArrayInternals& va, unsigned int bindingPoint, const BufferInternals& bf, int offset, int stride) const override;
    void unsetBindingPoint(VertexArrayInternals& va, unsigned int bindingPoint) const override;

    void connectAttributeToBindingPoint(VertexArrayInternals& va, unsigned int attribute, unsigned int bindingPoint) const override;

    void bind(const VertexArrayInternals& va) const override;
    void unbind(const VertexArrayInternals& va) const override;

    void renderArrays(const VertexArrayInternals& va, Primitive prim, int first, size_t count) const override;
    void renderArrays(const VertexArrayInternals& va, Primitive prim, int first, size_t count, int instancecount) const override;

    void renderElements(const VertexArrayInternals& va, Primitive prim, size_t count, IndexType type, const void* indices) const override;
    void renderElements(const VertexArrayInternals& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount) const override;

    void renderElementsBaseVertex(const VertexArrayInternals& va, Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const override;
    void renderElementsBaseVertex(const VertexArrayInternals& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const override;

private:

    void throwIfNotCurrentlyBound(const VertexArrayInternals& va) const;

#ifdef _DEBUG
    static inline unsigned int s_currentlyBoundID = 0u;
#endif // _DEBUG
};

}