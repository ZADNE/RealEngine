/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/textures/ImageView.hpp>

namespace re {

ImageView::ImageView(const vk::ImageViewCreateInfo& createInfo)
    : m_imageView{device().createImageView(createInfo)} {
}

ImageView::ImageView(ImageView&& other) noexcept
    : m_imageView(other.m_imageView) {
    other.m_imageView = nullptr;
}

ImageView& ImageView::operator=(ImageView&& other) noexcept {
    std::swap(m_imageView, other.m_imageView);
    return *this;
}

ImageView::~ImageView() {
    deletionQueue().enqueueDeletion(m_imageView);
}

} // namespace re