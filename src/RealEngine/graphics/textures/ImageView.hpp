/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec3.hpp>

#include <RealEngine/renderer/VulkanObjectBase.hpp>

namespace re {

/**
 * @brief Represents a view to an image
 */
class ImageView: public VulkanObjectBase {
public:
    /**
     * @brief Constructs a null ImageView that does not view any image
     */
    explicit ImageView() {}

    explicit ImageView(const vk::ImageViewCreateInfo& createInfo);

    ImageView(const ImageView&)            = delete;  /**< Noncopyable */
    ImageView& operator=(const ImageView&) = delete;  /**< Noncopyable */

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
