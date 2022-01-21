#include <RealEngine/graphics/UniformManager.hpp>

#include <RealEngine/resources/ResourceManager.hpp>

namespace RE {

	UniformManager UniformManager::std{};

	UniformManager::UniformManager() {

	}

	UniformManager::~UniformManager() {
		for (auto& ub : m_uniformBuffers) {
			glDeleteBuffers(1, &ub.second.first);
		}
	}

	void UniformManager::addUniformBuffer(const std::string& name, size_t size) {
		auto it = m_uniformBuffers.find(name);
		if (it == m_uniformBuffers.end()) {
			GLuint ubTemp;
			glCreateBuffers(1, &ubTemp);
			glNamedBufferData(ubTemp, size, NULL, GL_DYNAMIC_DRAW);
			glBindBufferRange(GL_UNIFORM_BUFFER, m_nextBI, ubTemp, 0, size);
			m_uniformBuffers.emplace(name, std::make_pair(ubTemp, m_nextBI));
			m_nextBI++;
		}
	}

	void UniformManager::addShader(const std::string& uniformBufferName, const ShaderProgram* program) {
		auto it = m_uniformBuffers.find(uniformBufferName);
		if (it != m_uniformBuffers.end()) {
			glUniformBlockBinding(program->getProgramID(), glGetUniformBlockIndex(program->getProgramID(), uniformBufferName.c_str()), it->second.second);
		}
	}

	void UniformManager::setUniformBuffer(const std::string& name, size_t offsetBytes, size_t countBytes, const void* data) {
		auto it = m_uniformBuffers.find(name);
		if (it != m_uniformBuffers.end()) {
			glNamedBufferSubData(it->second.first, offsetBytes, countBytes, data);
		}
	}

}