/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/resources/TextureCache.hpp>
#include <RealEngine/resources/ShaderProgramCache.hpp>


namespace RE {

/**
 * @brief Ensures there is at most one copy of shared resources.
 *
 * Resources managed by resource manager: textures, shader programs.
 * Resources are released once there are no references to them.
 * 
 * Also accessible through "RM" abbreviation.
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
	static ShaderProgramPtr getShaderProgram(const ShaderProgramSources& sources);

private:
	static TextureCache m_textureCache;
	static ShaderProgramCache m_shaderProgramCache;
};

using RM = class ResourceManager;

}