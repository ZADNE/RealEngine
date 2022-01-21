#pragma once
#include <unordered_map>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include <RealEngine/resources/ShaderProgramCache.hpp>

namespace RE {

	class UniformManager {
	public:
		UniformManager();
		~UniformManager();

		//Adds new uniform buffer to management
		void addUniformBuffer(const std::string& name, size_t size);

		//Adds new shader to management
		void addShader(const std::string& uniformBufferName, const ShaderProgram* program);

		//Uniform buffer first has to be added via addUniformBuffer()
		void setUniformBuffer(const std::string& name, size_t offsetBytes, size_t countBytes, const void* data);

		static UniformManager std;
	private:
		//								Uniform Buffer		Binding Index
		//										   \|/		\|/
		std::unordered_map<std::string, std::pair<GLuint, GLuint>> m_uniformBuffers;

		//std::unordered_map<std::string, GLuint> m_uniformBuffers;
		GLuint m_nextBI = 0u;
	};

}