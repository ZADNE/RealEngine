/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <concepts>

#include <glm/vec4.hpp>

#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Specifies parameters for CommandBuffer creation
 */
struct CommandBufferCreateInfo {
    // Level
    vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary;

    // Debug
    [[no_unique_address]] DebugName<> debugName;
};

/**
 * @brief Records commands to be executed by device (= GPU)
 */
class CommandBuffer: public VulkanObject {
public:
    /**
     * @brief Constructs a null CommandBuffer that cannot be used for recording
     */
    explicit CommandBuffer() {}

    explicit CommandBuffer(const CommandBufferCreateInfo& createInfo);

    CommandBuffer(const CommandBuffer&)            = delete; /**< Noncopyable */
    CommandBuffer& operator=(const CommandBuffer&) = delete; /**< Noncopyable */

    CommandBuffer(CommandBuffer&& other) noexcept;            /**< Movable */
    CommandBuffer& operator=(CommandBuffer&& other) noexcept; /**< Movable */

    ~CommandBuffer();

#pragma region Queue submission (static)

    /**
     * @warning Waits for device to become idle which is very expensive!
     *          Use only when performance is not critical (e.g. outside of main loop)
     */
    template<std::invocable<const CommandBuffer&> F>
    static void doOneTimeSubmit(F op) {
        oneTimeSubmitCmdBuf()->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit}
        );
        op(oneTimeSubmitCmdBuf());
        oneTimeSubmitCmdBuf()->end();
        graphicsQueue().submit(vk::SubmitInfo{{}, {}, *oneTimeSubmitCmdBuf()});
        device().waitIdle();
    }

    static void submitToGraphicsQueue(
        const vk::ArrayProxy<const vk::SubmitInfo2>& submits,
        const vk::Fence&                             signalFence = nullptr
    );

    static void submitToComputeQueue(
        const vk::ArrayProxy<const vk::SubmitInfo2>& submits,
        const vk::Fence&                             signalFence = nullptr
    );

    void submitToGraphicsQueue(const vk::Fence& signalFence = nullptr) const;
    void submitToComputeQueue(const vk::Fence& signalFence = nullptr) const;

#pragma endregion

#pragma region Debug support

    /**
     * @brief Begins a labeled debug region in the command buffer
     * @note  Does nothing in release build
     */
    void beginDebugRegion(const char* label, glm::vec4 color = {}) const {
        if constexpr (k_buildType == BuildType::Debug) {
            m_cmdBuf.beginDebugUtilsLabelEXT(
                vk::DebugUtilsLabelEXT{label, {color.r, color.g, color.b, color.a}},
                dispatchLoaderDynamic()
            );
        }
    }

    /**
     * @brief Ends a labeled debug region into the command buffer
     * @note  Does nothing in release build
     */
    void endDebugRegion() const {
        if constexpr (k_buildType == BuildType::Debug) {
            m_cmdBuf.endDebugUtilsLabelEXT(dispatchLoaderDynamic());
        }
    }

    /**
     * @brief Inserts a single debug label in the command buffer
     * @note  Does nothing in release build
     */
    void insertDebugLabel(const char* label, glm::vec4 color = {}) const {
        if constexpr (k_buildType == BuildType::Debug) {
            m_cmdBuf.insertDebugUtilsLabelEXT(
                vk::DebugUtilsLabelEXT{label, {color.r, color.g, color.b, color.a}},
                dispatchLoaderDynamic()
            );
        }
    }

    /**
     * @brief Is a RAII wrapper of labelled debug regions
     * @note  Effectively does nothing in release build
     */
    class [[nodiscard]] DebugRegion {
    public:
        DebugRegion(
            const re::CommandBuffer& cmdBuf, const char* label, glm::vec4 color = {}
        )
            : m_cmdBuf(cmdBuf) {
            m_cmdBuf.beginDebugRegion(label, color);
        }

        DebugRegion(const DebugRegion&)            = delete; /**< Noncopyable */
        DebugRegion& operator=(const DebugRegion&) = delete; /**< Noncopyable */

        DebugRegion(DebugRegion&&)            = delete; /**< Nonmovable */
        DebugRegion& operator=(DebugRegion&&) = delete; /**< Nonmovable */

        ~DebugRegion() { m_cmdBuf.endDebugRegion(); }

    private:
        const re::CommandBuffer& m_cmdBuf;
    };

    DebugRegion createDebugRegion(const char* label, glm::vec4 color = {}) const {
        return DebugRegion{*this, label, color};
    }

#pragma endregion

    const vk::CommandBuffer& operator*() const { return m_cmdBuf; }
    const vk::CommandBuffer* operator->() const { return &m_cmdBuf; }

    const vk::CommandBuffer& commandBuffer() const { return m_cmdBuf; }

private:
    vk::CommandBuffer m_cmdBuf{};
};

} // namespace re
