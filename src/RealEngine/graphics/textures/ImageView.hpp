/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec3.hpp>

#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Represents a view to an image
 */
class ImageView: public VulkanObject {
public:
    explicit ImageView(const vk::ImageViewCreateInfo& createInfo);

    ImageView(const ImageView&)            = delete; /**< Noncopyable */
    ImageView& operator=(const ImageView&) = delete; /**< Noncopyable */

    ImageView(ImageView&& other) noexcept;            /**< Movable */
    ImageView& operator=(ImageView&& other) noexcept; /**< Movable */

    ~ImageView();

    const vk::ImageView& operator*() const { return m_imageView; }
    const vk::ImageView* operator->() const { return &m_imageView; }

    const vk::ImageView& imageView() const { return m_imageView; }

protected:
    vk::ImageView m_imageView{};
};

static_assert(sizeof(ImageView) == sizeof(vk::ImageView));
static_assert(alignof(ImageView) == alignof(vk::ImageView));

} // namespace re
