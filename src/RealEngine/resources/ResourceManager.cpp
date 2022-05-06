/*! 
 *  \author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceManager.hpp>

namespace RE {

TextureCache ResourceManager::m_textureCache{};
ShaderProgramCache ResourceManager::m_shaderProgramCache{};


TexturePtr ResourceManager::getTexture(const std::string& filePath) {
	return m_textureCache.getTexture(filePath);
}

ShaderProgramPtr ResourceManager::getShaderProgram(const ShaderProgramSources& sources) {
	return m_shaderProgramCache.getShaderProgram(sources);
}

}