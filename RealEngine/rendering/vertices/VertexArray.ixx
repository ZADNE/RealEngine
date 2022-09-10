/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.vertices.VertexArray;
export import RealEngine.rendering.internal_interfaces.IVertexArray;
import RealEngine.rendering.buffers.Buffer;
export import RealEngine.rendering.Renderer;


export namespace RE {

/**
 * @brief Controls how & where vertices are pulled
 * @tparam R The renderer that will perform the commands
 *
 * A ShaderProgram has to be used in conjunction with this.
*/
template<Renderer R = RendererLateBind>
class VertexArray {
    friend class GL46Fixture;
public:

    /**
     * @brief Constructs a blank vertex array that does not pull any vertices.
    */
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray<R>&) = delete;
    VertexArray(VertexArray<R>&& other) noexcept;

    VertexArray<R>& operator=(const VertexArray<R>&) = delete;
    VertexArray<R>& operator=(VertexArray<R>&& other) noexcept;


    void setAttribute(unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize = true);
    void unsetAttribute(unsigned int attribute);

    void setBindingPoint(unsigned int bindingPoint, const Buffer<R>& buffer, int offset, int stride);
    void unsetBindingPoint(unsigned int bindingPoint);

    void connectAttributeToBindingPoint(unsigned int attribute, unsigned int bindingPoint);

    void bind() const;
    void unbind() const;

    void renderArrays(Primitive prim, int first, size_t count) const;
    void renderArrays(Primitive prim, int first, size_t count, int instancecount) const;

    void renderElements(Primitive prim, size_t count, IndexType type, const void* indices) const;
    void renderElements(Primitive prim, size_t count, IndexType type, const void* indices, int instancecount) const;

    void renderElementsBaseVertex(Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const;
    void renderElementsBaseVertex(Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const;

private:

    VertexArrayID m_id;

    static inline R::VertexArray* s_impl = nullptr;
};

}