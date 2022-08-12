/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cassert>

#include <RealEngine/rendering/internal_interfaces/IFramebuffer.hpp>


namespace RE {

/**
* @brief Controls where output of rendering is stored
*
* Framebuffers can be used to render to textures,
* instead of rendering directly to window.
*/
class Framebuffer {
	friend class GL46_Renderer;
public:

	/**
	 * @brief Use this to access the default framebuffer (the visible window)
	*/
	static Framebuffer defaultFramebuffer;

	/**
	 * @brief Constructs new framebuffer
	*/
	Framebuffer();

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer(Framebuffer&& other) noexcept;

	Framebuffer& operator=(const Framebuffer&) = delete;
	Framebuffer& operator=(Framebuffer&& other) noexcept;

	/**
	 * @brief Destroys the framebuffer
	 *
	 * This does not destroy the connected textures.
	*/
	~Framebuffer();


private:

	/**
	 * @brief Used only to contruct the default framebuffer
	*/
	Framebuffer(unsigned int ID) : m_ID(ID) { assert(m_ID == 0); }

	unsigned int m_ID;

	static IFramebuffer* s_impl;
};

}