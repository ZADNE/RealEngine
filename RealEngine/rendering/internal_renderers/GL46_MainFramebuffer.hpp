/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IMainFramebuffer.hpp>

namespace RE {

/**
 * @brief Controls transformation from Normalized device coordinates to Window coordinates
*/
class GL46_MainFramebuffer : public IMainFramebuffer {
	friend class GL46_Renderer;
	friend class GL46_Buffer;
public:

    void setClearColor(const glm::vec4& color) const override;
    void setClearDepth(float depth) const override;
    void setClearDepth(double depth) const override;
    void setClearStencil(int stencil) const override;

    void clear(ClearFlags mask) const override;

};

}