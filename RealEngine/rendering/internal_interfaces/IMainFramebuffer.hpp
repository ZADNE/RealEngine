/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <type_traits>

#include <glm/vec4.hpp>

namespace RE {

/**
 * @brief Determines which buffers of the framebuffer should be cleared
*/
enum class ClearFlags: unsigned int {
    NONE = 0,
    COLOR = 1,
    DEPTH = 2,
    STENCIL = 4
};

inline ClearFlags operator|(ClearFlags a, ClearFlags b) {
    return static_cast<ClearFlags>(static_cast<std::underlying_type_t<ClearFlags>>(a) | static_cast<std::underlying_type_t<ClearFlags>>(b));
}

inline bool operator&(ClearFlags a, ClearFlags b) {
    return static_cast<std::underlying_type_t<ClearFlags>>(a) & static_cast<std::underlying_type_t<ClearFlags>>(b);
}

/**
* @brief Is a renderer-agnostic interface to MainFramebuffer's implementation.
*
* This is used internally by MainFramebuffer.
*
* @see RE::MainFramebuffer
*/
class IMainFramebuffer {
public:

	virtual void setClearColor(const glm::vec4& color) const = 0;
    virtual void setClearDepth(float depth) const = 0;
    virtual void setClearDepth(double depth) const = 0;
    virtual void setClearStencil(int stencil) const = 0;

    virtual void clear(ClearFlags mask) const = 0;

};

}