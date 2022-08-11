/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IMainFramebuffer.hpp>

namespace RE {

/**
 * @brief Clears the whole window
*/
class MainFramebuffer {
	friend class GL46_Renderer;
public:

    /**
     * @brief Sets the color that will be used for clearing of color buffers
    */
    static void setClearColor(const glm::vec4& color);

    /**
     * @brief Sets the depth that will be used for clearing of depth buffers
    */
    static void setClearDepth(float depth);

    /**
     * @brief Sets the depth that will be used for clearing of depth buffers
    */
    static void setClearDepth(double depth);

    /**
     * @brief Sets the stencil that will be used for clearing of stencil buffers
    */
    static void setClearStencil(int stencil);

    /**
     * @brief Clears given buffers
     * @param mask Or'd mask of buffers to clear
    */
    static void clear(ClearFlags mask);

private:

    static IMainFramebuffer* s_impl;
};

}