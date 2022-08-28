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
class GL46_VertexArray : public IVertexArray {
public:

    void construct(VertexArray& va) const override;
    void destruct(VertexArray& va) const override;

    void setAttribute(VertexArray& va, unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize = true) const override;
    void unsetAttribute(VertexArray& va, unsigned int attribute) const override;

    void setBindingPoint(VertexArray& va, unsigned int bindingPoint, const Buffer& buffer, int offset, int stride) const override;
    void unsetBindingPoint(VertexArray& va, unsigned int bindingPoint) const override;

    void connectAttributeToBindingPoint(VertexArray& va, unsigned int attribute, unsigned int bindingPoint) const override;

    void bind(const VertexArray& va) const override;
    void unbind(const VertexArray& va) const override;

    void renderArrays(const VertexArray& va, Primitive prim, int first, size_t count) const override;
    void renderArrays(const VertexArray& va, Primitive prim, int first, size_t count, int instancecount) const override;

    void renderElements(const VertexArray& va, Primitive prim, size_t count, IndexType type, const void* indices) const override;
    void renderElements(const VertexArray& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount) const override;

    void renderElementsBaseVertex(const VertexArray& va, Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const override;
    void renderElementsBaseVertex(const VertexArray& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const override;

private:

    void throwIfNotCurrentlyBound(const VertexArray& va) const;

#ifdef _DEBUG
    static inline unsigned int s_currentlyBoundID = 0u;
#endif // _DEBUG
};

}