/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <glm/vec2.hpp>

namespace RE {

/**
* @brief Is a renderer-agnostic interface to Viewport implementation.
*
* This is used internally by Viewport.
*
* @see RE::Viewport
*/
class IViewport {
public:

	virtual void set(const glm::ivec2& pos, const glm::ivec2& size) const = 0;

};

}