#include <RealEngine/resources/ShaderProgramCache.hpp>

namespace RE {

ShaderProgramPtr ShaderProgramCache::getShaderProgram(const ShaderProgramSource& source) {
	auto mapIterator = m_shaderMap.find(source);
	ShaderProgramPtr stored;
	if (mapIterator != m_shaderMap.end() && (stored = mapIterator->second.lock())) {
		return stored;
	} else {//Shader never accessed before or it has expired
		auto shared_p = std::make_shared<ShaderProgram>(source);
		m_shaderMap.insert_or_assign(source, shared_p);
		return shared_p;
	}
}

}