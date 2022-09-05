/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IViewport.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Viewport class.
*
* Do not use this directly - use Viewport instead.
*/
class GL46_Viewport final : public IViewport {
public:

    void set(const glm::ivec2& pos, const glm::ivec2& size) const override;
};

}