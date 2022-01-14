#include <RealEngine/ShaderProgramCache.hpp>

#include <RealEngine/default_shaders.hpp>

namespace RE {

	ShaderProgramSeed ShaderProgramSeed::stdSprite{ {.vert = vert_sprite, .frag = frag_sprite} };
	ShaderProgramSeed ShaderProgramSeed::stdGeometry{ {.vert = vert_geometry, .frag = frag_geometry} };

	ShaderProgramPtr ShaderProgramCache::getShaderProgram(const ShaderProgramSeed& shaderSeed) {
		auto mapIterator = m_shaderMap.find(shaderSeed);
		ShaderProgramPtr stored;
		if (mapIterator != m_shaderMap.end() && (stored = mapIterator->second.lock())) {
			return stored;
		}
		else {//Shader never accessed before or it has expired
			auto shared_p = std::make_shared<ShaderProgram>(shaderSeed.source);
			m_shaderMap.insert_or_assign(shaderSeed, shared_p);
			return shared_p;
		}
	}

}