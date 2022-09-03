/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IFramebuffer.hpp>
#include <RealEngine/rendering/RendererLateBind.hpp>


namespace RE {

/**
 * @brief Controls where output of rendering is stored
 * @tparam R The renderer that will perform the commands
 *
 * Framebuffers can be used to render to textures,
 * instead of rendering directly to window.
*/
template<typename R = RendererLateBind>
class Framebuffer {
    friend class GL46_Fixture;
    friend class GL46_Framebuffer;
public:

    /**
     * @brief Constructs new framebuffer
    */
    Framebuffer();

    Framebuffer(const Framebuffer<R>&) = delete;
    Framebuffer(Framebuffer<R>&& other) noexcept;

    Framebuffer<R>& operator=(const Framebuffer<R>&) = delete;
    Framebuffer<R>& operator=(Framebuffer<R>&& other) noexcept;

    /**
     * @brief Destroys the framebuffer
     *
     * This does not destroy the connected textures.
    */
    ~Framebuffer();

    /**
     * @brief Attaches an image to the framebuffer so that it can be used as target.
    */
    void attachImage(FramebufferAttachment attachment, const Texture<R>& te, int level);

    /**
     * @brief Associates indexed shader color outputs with attachments.
     *
     * All outputs on indexes after the last provided are discarded.
     * @param outputs N-th element represents n-th indexed output, the value selects the attachment.
    */
    void associateAttachementsWithOutputs(const std::vector<FramebufferOutput>& outputs);

    /**
     * @brief Selects which of the color attachments will be used as source of pixel reading.
    */
    void selectAttachmentForColorReading(unsigned int colorAttachmentIndex);

    /**
     * @brief Sets the framebuffer as target for further operations.
    */
    void targetMe(FramebufferTarget target) const;

    /**
     * @brief Tests whether the framebuffer can be targetted via targetMe().
     *
     * Framebuffer can get into invalid state via incorrect usage of attachImage(),
     * associateAttachementsWithOutputs() or selectAttachmentForColorReading() functions.
     * @return FramebufferTargetability::TARGETABLE when can be targetted.
    */
    FramebufferTargetability checkTargetability(FramebufferTarget target) const;

    void clearColorAttachment(unsigned int attachmentIndex, const glm::vec4& color) const;
    void clearColorAttachment(unsigned int attachmentIndex, const glm::ivec4& color) const;
    void clearColorAttachment(unsigned int attachmentIndex, const glm::uvec4& color) const;
    void clearDepthAttachment(float depth) const;
    void clearStencilAttachment(int stencil) const;
    void clearDepthAndStencilAttachments(float depth, int stencil) const;

private:

    /**
     * @brief Used only to contruct the default framebuffer
    */
    Framebuffer(FramebufferID&& id);

    FramebufferID m_id;

    static inline R::Framebuffer* s_impl = nullptr;
};

/**
 * @brief Allows retargeting to and clearing of the main window canvas
 * @tparam R The renderer that will perform the commands
*/
template<typename R = RendererLateBind>
class DefaultFrameBuffer {
    friend class GL46_Fixture;
public:

    /**
     * @copydoc Framebuffer::targetMe()
    */
    static void targetMe(FramebufferTarget target);

    static void clearColor(const glm::vec4& color);
    static void clearColor(const glm::ivec4& color);
    static void clearColor(const glm::uvec4& color);
    static void clearDepth(float depth);
    static void clearStencil(int stencil);
    static void clearDepthAndStencil(float depth, int stencil);

private:

    static inline Framebuffer<R>* s_defaultFramebuffer = nullptr;
};

}