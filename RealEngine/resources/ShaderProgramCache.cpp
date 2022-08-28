/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ShaderProgramCache.hpp>

namespace RE {

ShaderProgramPtr ShaderProgramCache::getShaderProgram(const ShaderProgramSources& sources) {
    auto mapIterator = m_shaderMap.find(sources);
    ShaderProgramPtr stored;
    if (mapIterator != m_shaderMap.end() && (stored = mapIterator->second.lock())) {
        return stored;
    } else {//Shader never accessed before or it has expired
        auto shared_p = std::make_shared<ShaderProgram>(sources);
        m_shaderMap.insert_or_assign(sources, shared_p);
        return shared_p;
    }
}

}