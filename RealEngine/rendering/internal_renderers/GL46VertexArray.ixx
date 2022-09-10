/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.internal_renderers.GL46VertexArray;
import RealEngine.rendering.internal_interfaces.IVertexArray;

export namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of VertexArray class.
*
* Do not use this directly - use VertexArray instead.
*/
class GL46VertexArray final : public IVertexArray {
public:

    VertexArrayID construct() const override;
    void destruct(VertexArrayID& va) const override;

    void setAttribute(VertexArrayID& va, unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize = true) const override;
    void unsetAttribute(VertexArrayID& va, unsigned int attribute) const override;

    void setBindingPoint(VertexArrayID& va, unsigned int bindingPoint, const BufferID& bf, int offset, int stride) const override;
    void unsetBindingPoint(VertexArrayID& va, unsigned int bindingPoint) const override;

    void connectAttributeToBindingPoint(VertexArrayID& va, unsigned int attribute, unsigned int bindingPoint) const override;

    void bind(const VertexArrayID& va) const override;
    void unbind(const VertexArrayID& va) const override;

    void renderArrays(const VertexArrayID& va, Primitive prim, int first, size_t count) const override;
    void renderArrays(const VertexArrayID& va, Primitive prim, int first, size_t count, int instancecount) const override;

    void renderElements(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices) const override;
    void renderElements(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount) const override;

    void renderElementsBaseVertex(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const override;
    void renderElementsBaseVertex(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const override;

private:

    void throwIfNotCurrentlyBound(const VertexArrayID& va) const;

#ifdef _DEBUG
    static inline unsigned int s_currentlyBoundID = 0u;
#endif // _DEBUG
};

}