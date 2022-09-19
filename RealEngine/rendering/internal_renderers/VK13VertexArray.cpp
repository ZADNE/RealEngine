/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13VertexArray.hpp>

namespace RE {

VertexArrayID VK13VertexArray::construct() const {
    return VertexArrayID{0};
}

void VK13VertexArray::destruct(VertexArrayID& va) const {

}

void VK13VertexArray::setAttribute(VertexArrayID& va, unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize/* = true*/) const {

}

void VK13VertexArray::unsetAttribute(VertexArrayID& va, unsigned int attribute) const {

}

void VK13VertexArray::setBindingPoint(VertexArrayID& va, unsigned int bindingPoint, const BufferID& bf, int offset, int stride) const {

}

void VK13VertexArray::unsetBindingPoint(VertexArrayID& va, unsigned int bindingPoint) const {

}

void VK13VertexArray::connectAttributeToBindingPoint(VertexArrayID& va, unsigned int attribute, unsigned int bindingPoint) const {

}

void VK13VertexArray::bind(const VertexArrayID& va) const {

}

void VK13VertexArray::unbind(const VertexArrayID& va) const {

}

void VK13VertexArray::renderArrays(const VertexArrayID& va, Primitive prim, int first, size_t count) const {

}

void VK13VertexArray::renderArrays(const VertexArrayID& va, Primitive prim, int first, size_t count, int instancecount) const {

}

void VK13VertexArray::renderElements(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices) const {

}

void VK13VertexArray::renderElements(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount) const {

}

void VK13VertexArray::renderElementsBaseVertex(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const {

}

void VK13VertexArray::renderElementsBaseVertex(const VertexArrayID& va, Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const {

}

}