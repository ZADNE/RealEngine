/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/buffers/Buffer.hpp>
#include <RealEngine/graphics/commands/CommandBuffer.hpp>
#include <RealEngine/graphics/textures/Texture.hpp>

namespace re {

/**
 * @brief Describes an access to a named buffer
 * @details This is used by ActionCommandBuffer
 */
template<typename BufferName>
    requires std::is_enum_v<BufferName>
struct BufferAccess {
    BufferName              name;
    vk::PipelineStageFlags2 stage;
    vk::AccessFlags2        access;
};

/**
 * @brief Describes an access to a named image
 * @details This is used by ActionCommandBuffer
 */
template<typename ImageName>
    requires std::is_enum_v<ImageName>
struct ImageAccess {
    ImageName               name;
    vk::PipelineStageFlags2 stage;
    vk::AccessFlags2        access;
    vk::ImageLayout         layout = vk::ImageLayout::eGeneral;
};

/**
 * @brief Is either BufferAccess or ImageAccess
 */
template<typename T, typename BufferName, typename ImageName>
concept ResourceAccess = std::is_same_v<BufferAccess<BufferName>, T> ||
                         std::is_same_v<ImageAccess<ImageName>, T>;

/**
 * @brief   Prevents harazardous accesses to buffers and images
 * @tparam  BufferName An enum representing the tracked buffers
 * @tparam  k_trackedBufferCount Values of BufferName must be in range
 *          <0, k_trackedBufferCount>
 * @tparam  ImageName An enum representing the tracked images
 * @tparam  k_trackedImageCount Values of ImageName must be in range
 *          <0, k_trackedImageCount>
 * @details ActionCommandBuffer assumes that the tracked resources are only
 *          accessed from within one queue
 * @details All harards are resolved by vkCmdPipelineBarrier2.
 */
template<typename BufferName, size_t k_trackedBufferCount, typename ImageName, size_t k_trackedImageCount>
    requires std::is_enum_v<BufferName> && std::is_enum_v<ImageName>
class ActionCommandBuffer {
public:
    /**
     * @note The actual CommandBuffer must be set before ACB can be used
     * @note Actual buffers and images must be assigned to names before they can
     * be tracked
     */
    explicit ActionCommandBuffer() {}

    /**
     * @brief Assigns concrete buffer to a buffer name
     */
    void track(BufferName name, const re::Buffer& buf) {
        state(name) = BufferState{buf.buffer(), {}, {}};
    }

    /**
     * @brief Assigns concrete image to an image name
     */
    void track(
        ImageName          name,
        const re::Texture& tex,
        vk::ImageLayout    layout,
        uint32_t           layerCount = 1
    ) {
        state(name) = ImageState{tex.image(), {}, {}, layout, layerCount};
    }

    /**
     * @brief Assigns an actuall command buffer to be used
     * @details The command buffer can be temporarily overridden by a a secondary one
     */
    void useCommandBuffer(const CommandBuffer& cb) { m_cb = &cb; }

    void useSecondaryCommandBuffer(const CommandBuffer& cb) const {
        assert(m_secCb == nullptr);
        m_secCb = &cb;
    }

    void stopSecondaryCommandBuffer() const { m_secCb = nullptr; }

    /**
     * @brief Performs an action seperated by a pipeline barrier if necessary
     * @param action A lambda like that does the actual work
     * @param ...accesses Accesses to tracked buffers and images
     */
    template<std::invocable<const CommandBuffer&> Action, ResourceAccess<BufferName, ImageName>... Accesses>
    void action(Action action, Accesses... accesses) const {
        insertBarrierIfNecessary(accesses...);
        action(*cb());
    }

    void assumeActionsFinished() {
        for (auto& buf : m_bufferStates) {
            buf.lastStage  = {};
            buf.lastAccess = {};
        }
        for (auto& img : m_imageStates) {
            img.lastStage  = {};
            img.lastAccess = {};
        }
    }

    const re::CommandBuffer& operator*() const { return *cb(); }
    const re::CommandBuffer* operator->() const { return cb(); }

    const re::CommandBuffer& commandBuffer() const { return *cb(); }

private:
    const CommandBuffer*         m_cb    = nullptr;
    mutable const CommandBuffer* m_secCb = nullptr; // Secondary command buffer

    const CommandBuffer* cb() const { return m_secCb ? m_secCb : m_cb; }

    struct BufferState {
        vk::Buffer              buffer{};
        vk::PipelineStageFlags2 lastStage{};
        vk::AccessFlags2        lastAccess{};
    };
    mutable std::array<BufferState, k_trackedBufferCount> m_bufferStates;

    struct ImageState {
        vk::Image               image{};
        vk::PipelineStageFlags2 lastStage{};
        vk::AccessFlags2        lastAccess{};
        vk::ImageLayout         layout{};
        uint32_t                layerCount{};
    };
    mutable std::array<ImageState, k_trackedImageCount> m_imageStates;

    mutable std::vector<vk::BufferMemoryBarrier2> m_bufferBarriers;
    mutable std::vector<vk::ImageMemoryBarrier2>  m_imageBarriers;

    static constexpr vk::AccessFlags2 k_writeAccessBits =
        vk::AccessFlagBits2::eShaderWrite |
        vk::AccessFlagBits2::eColorAttachmentWrite |
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite |
        vk::AccessFlagBits2::eTransferWrite | vk::AccessFlagBits2::eHostWrite |
        vk::AccessFlagBits2::eMemoryWrite |
        vk::AccessFlagBits2::eShaderStorageWrite;

    static constexpr bool isHazardAccess(vk::AccessFlags2 prev, vk::AccessFlags2 next) {
        // True unless it is read-after-read conflict
        return (prev & k_writeAccessBits) || (next & k_writeAccessBits);
    }

    BufferState& state(BufferName name) const {
        auto i = std::to_underlying(name);
        assert(i < k_trackedBufferCount);
        return m_bufferStates[i];
    }

    ImageState& state(ImageName name) const {
        auto i = std::to_underlying(name);
        assert(i < k_trackedImageCount);
        return m_imageStates[i];
    }

    template<ResourceAccess<BufferName, ImageName>... Accesses>
    void insertBarrierIfNecessary(Accesses... accesses) const {
        m_bufferBarriers.clear();
        m_imageBarriers.clear();
        composeBarriers(accesses...);
        if (!m_bufferBarriers.empty() || !m_imageBarriers.empty()) {
            (*cb())->pipelineBarrier2(vk::DependencyInfo{
                {}, {}, m_bufferBarriers, m_imageBarriers});
        }
    }

    template<ResourceAccess<BufferName, ImageName>... Accesses>
    void composeBarriers(BufferAccess<BufferName> access, Accesses... accesses) const {
        BufferState& last = state(access.name);
        if (isHazardAccess(last.lastAccess, access.access)) {
            m_bufferBarriers.emplace_back(
                last.lastStage,
                last.lastAccess,
                access.stage,
                access.access,
                vk::QueueFamilyIgnored,
                vk::QueueFamilyIgnored,
                last.buffer,
                0,
                vk::WholeSize
            );
            last.lastStage  = access.stage;
            last.lastAccess = access.access;
        } else {
            last.lastStage |= access.stage;
            last.lastAccess |= access.access;
        }
        composeBarriers(accesses...);
    }

    template<ResourceAccess<BufferName, ImageName>... Accesses>
    void composeBarriers(ImageAccess<ImageName> access, Accesses... accesses) const {
        ImageState& last = state(access.name);
        if (isHazardAccess(last.lastAccess, access.access) ||
            (last.layout != access.layout)) {
            m_imageBarriers.emplace_back(
                last.lastStage,
                last.lastAccess,
                access.stage,
                access.access,
                last.layout,
                access.layout,
                vk::QueueFamilyIgnored,
                vk::QueueFamilyIgnored,
                last.image,
                vk::ImageSubresourceRange{
                    vk::ImageAspectFlagBits::eColor, 0, 1, 0, last.layerCount}
            );
            last.lastStage  = access.stage;
            last.lastAccess = access.access;
            last.layout     = access.layout;
        } else {
            last.lastStage |= access.stage;
            last.lastAccess |= access.access;
        }
        composeBarriers(accesses...);
    }

    template<ResourceAccess<BufferName, ImageName>... Accesses>
    void composeBarriers() const {}
};

} // namespace re
