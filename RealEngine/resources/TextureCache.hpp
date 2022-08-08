/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <unordered_map>
#include <memory>

#include <RealEngine/rendering/textures/Texture.hpp>

namespace RE {

using TexturePtr = std::shared_ptr<const Texture>;

class TextureCache {
public:
	TextureCache() {}

	/**
	 * @brief Gets texture as a shared resource.
	 * @param filePathNoExt Path to the texture (without extension)
	 * @return Texture as a shared resource
	*/
	TexturePtr getTexture(const std::string& filePathNoExt);
private:
	std::unordered_map<std::string, std::weak_ptr<Texture>> m_textureMap;
	std::string m_textureFolder = "textures/";
};

}