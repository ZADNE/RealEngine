#pragma once
#include <RealEngine/resources/TextureCache.hpp>
#include <RealEngine/resources/ShaderProgramCache.hpp>
#include <RealEngine/resources/FontCache.hpp>


namespace RE {

/**
 * @brief Resource Manager (also as accessible through shortcut RM) is
 * pure static class that manages shared resources.
 *
 * Resources managed by resource meneger: textures, shader programs, fonts.
 * Resources are released once there are no references to them.
*/
class ResourceManager {
public:

	/**
	 * @copydoc TextureCache::getTexture
	*/
	static TexturePtr getTexture(const std::string& filePath);

	/**
	 * @copydoc ShaderProgramCache::getShaderProgram
	*/
	static ShaderProgramPtr getShaderProgram(const ShaderProgramSource& source);

	//Font
	static const Font* getFont(const FontSeed& fontSeed);
	static void removeFont(const FontSeed& fontSeed);
private:
	static TextureCache m_textureCache;
	static ShaderProgramCache m_shaderProgramCache;
	static FontCache m_fontCache;
};

using RM = class ResourceManager;

}