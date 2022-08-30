/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IVertexArray.hpp>

namespace RE {

/**
 * @brief Controls how & where vertices are pulled.
 *
 * A ShaderProgram has to be used in conjunction with this.
*/
class VertexArray {
    friend class GL46_Fixture;
    friend class GL46_VertexArray;
public:

    /**
     * @brief Constructs a blank vertex array that does not pull any vertices.
    */
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray(VertexArray&& other) noexcept;

    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray& operator=(VertexArray&& other) noexcept;


    void setAttribute(unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize = true);
    void unsetAttribute(unsigned int attribute);

    void setBindingPoint(unsigned int bindingPoint, const Buffer& buffer, int offset, int stride);
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

    unsigned int m_ID = 0;      /**< Internal identifier*/

    static IVertexArray* s_impl;
};

}