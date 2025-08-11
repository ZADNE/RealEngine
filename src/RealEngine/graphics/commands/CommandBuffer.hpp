/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <concepts>

#include <glm/vec4.hpp>

#include <RealEngine/graphics/commands/BarrierHelperFuncs.hpp>
#include <RealEngine/renderer/ObjectUsingVulkan.hpp>

namespace re {

/**
 * @brief Specifies parameters for CommandBuffer creation
 */
struct CommandBufferCreateInfo {
    // Level
    vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary;

    // Debug
    [[no_unique_address]] DebugString<> debugName;
};

/**
 * @brief Records commands to be executed by device (= GPU)
 */
class CommandBuffer: public ObjectUsingVulkan {
public:
    /**
     * @brief Constructs a null CommandBuffer that cannot be used for recording
     */
    explicit CommandBuffer() {}

    explicit CommandBuffer(const CommandBufferCreateInfo& createInfo);

    CommandBuffer(const CommandBuffer&)            = delete;  ///< Noncopyable
    CommandBuffer& operator=(const CommandBuffer&) = delete;  ///< Noncopyable

    CommandBuffer(CommandBuffer&& other) noexcept;            ///< Movable
    CommandBuffer& operator=(CommandBuffer&& other) noexcept; ///< Movable

    ~CommandBuffer();

#pragma region Queue submission

    /**
     * @brief Records a command buffer and submits it
     * @warning Waits for device to become idle which is very expensive!
     *          Use only when performance is not critical (e.g. outside of main
     * loop)
     */
    template<std::invocable<const CommandBuffer&> F>
    static void doOneTimeSubmit(F op) {
        oneTimeSubmitCmdBuf()->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit
        });
        op(oneTimeSubmitCmdBuf());
        oneTimeSubmitCmdBuf()->end();
        graphicsCompQueue().submit(vk::SubmitInfo{{}, {}, *oneTimeSubmitCmdBuf()});
        device().waitIdle();
    }

    /**
     * @brief Submits the work to a queue which support graphics, compute and
     * transfer work
     */
    static void submitToGraphicsCompQueue(
        const vk::ArrayProxy<const vk::SubmitInfo2>& submits,
        const vk::Fence& signalFence = nullptr
    );

    /**
     * @brief Submits the command buffer to a queue which support graphics,
     * compute and transfer work
     */
    void submitToGraphicsCompQueue(const vk::Fence& signalFence = nullptr) const;

#pragma endregion

#pragma region Debug support

    /**
     * @brief Inserts a global memory all-commands to all-commands barrier
     * @note  Use for debug purposes only, do not use for release
     */
    void debugBarrier() const;

    /**
     * @brief Begins a labeled debug region in the command buffer
     * @note  Does nothing in release build
     */
    void beginDebugRegion(const char* label, glm::vec4 color = {}) const {
#if RE_BUILDING_FOR_DEBUG
        m_cb.beginDebugUtilsLabelEXT(
            vk::DebugUtilsLabelEXT{label, {color.r, color.g, color.b, color.a}},
            dispatchLoaderDynamic()
        );
#endif // RE_BUILDING_FOR_DEBUG
    }

    /**
     * @brief Ends a labeled debug region into the command buffer
     * @note  Does nothing in release build
     */
    void endDebugRegion() const {
#if RE_BUILDING_FOR_DEBUG
        m_cb.endDebugUtilsLabelEXT(dispatchLoaderDynamic());
#endif // RE_BUILDING_FOR_DEBUG
    }

    /**
     * @brief Inserts a single debug label in the command buffer
     * @note  Does nothing in release build
     */
    void insertDebugLabel(const char* label, glm::vec4 color = {}) const {
#if RE_BUILDING_FOR_DEBUG
        m_cb.insertDebugUtilsLabelEXT(
            vk::DebugUtilsLabelEXT{label, {color.r, color.g, color.b, color.a}},
            dispatchLoaderDynamic()
        );
#endif // RE_BUILDING_FOR_DEBUG
    }

    /**
     * @brief Is a RAII wrapper of labelled debug regions
     * @note  Effectively does nothing in release build
     */
    class [[nodiscard]] DebugRegion {
    public:
        DebugRegion(const CommandBuffer& cb, const char* label, glm::vec4 color = {})
            : m_cb(cb) {
            m_cb.beginDebugRegion(label, color);
        }

        DebugRegion(const DebugRegion&)            = delete; ///< Noncopyable
        DebugRegion& operator=(const DebugRegion&) = delete; ///< Noncopyable

        DebugRegion(DebugRegion&&)            = delete;      ///< Nonmovable
        DebugRegion& operator=(DebugRegion&&) = delete;      ///< Nonmovable

        ~DebugRegion() { m_cb.endDebugRegion(); }

    private:
        const CommandBuffer& m_cb;
    };

    DebugRegion createDebugRegion(const char* label, glm::vec4 color = {}) const {
        return DebugRegion{*this, label, color};
    }

#pragma endregion

    const vk::CommandBuffer& operator*() const { return m_cb; }
    const vk::CommandBuffer* operator->() const { return &m_cb; }

    const vk::CommandBuffer& commandBuffer() const { return m_cb; }

private:
    vk::CommandBuffer m_cb{};
};

} // namespace re
