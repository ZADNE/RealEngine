﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/graphics/renderers/IViewport.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Viewport
*
* Do not use this directly - use Viewport class instead.
*/
class GL46_Viewport : public IViewport {
public:

	void set(const glm::ivec2& pos, const glm::ivec2& size) const override;
};

}