#include <RealEngine/resources/ResourceManager.hpp>

namespace RE {

TextureCache ResourceManager::m_textureCache{};
ShaderProgramCache ResourceManager::m_shaderProgramCache{};
FontCache ResourceManager::m_fontCache{};


TexturePtr ResourceManager::getTexture(const std::string& filePath) {
	return m_textureCache.getTexture(filePath);
}

ShaderProgramPtr ResourceManager::getShaderProgram(const ShaderProgramSource& source) {
	return m_shaderProgramCache.getShaderProgram(source);
}

const Font* ResourceManager::getFont(const FontSeed& fontSeed) {
	return m_fontCache.getFont(fontSeed);
}

void ResourceManager::removeFont(const FontSeed& fontSeed) {
	m_fontCache.removeFont(fontSeed);
}

}