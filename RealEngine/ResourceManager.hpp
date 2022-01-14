#pragma once
#include <RealEngine/TextureCache.hpp>
#include <RealEngine/ShaderProgramCache.hpp>
#include <RealEngine/FontCache.hpp>


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
		 * @brief Gets texture as a shared resource.
		 * @param filePath Path to the texture (without extension)
		 * @return Texture as shared resource
		*/
		static TexturePtr getTexture(const std::string& filePath);

		/**
		 * @brief Gets shader program as a shared resource.
		 * @param shaderSeed Seed to construct the shader from
		 * @return Shader program as shared resource
		*/
		static ShaderProgramPtr getShaderProgram(const ShaderProgramSeed& shaderSeed);

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