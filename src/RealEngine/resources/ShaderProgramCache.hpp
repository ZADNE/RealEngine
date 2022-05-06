/*! 
 *  \author    Dubsky Tomas
 */
#pragma once
#include <unordered_map>
#include <memory>

#include <RealEngine/graphics/ShaderProgram.hpp>

/**
 * @author The boost library
*/
inline void hash_combine(std::size_t& seed) { }
template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	hash_combine(seed, rest...);
}

namespace std {
template <> struct hash<RE::ShaderSources> {
	size_t operator()(const RE::ShaderSources& sources) const {
		std::size_t hash = sources.m_sources.size();
		for (auto& i : sources.m_sources) {
			hash ^= reinterpret_cast<intptr_t>(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
		return hash;
	}
};
}

namespace std {
template <> struct hash<RE::ShaderProgramSources> {
	size_t operator()(const RE::ShaderProgramSources& source) const {
		std::size_t hash = 0;
		hash_combine(hash,
			source.vert, source.tesc, source.tese,
			source.geom, source.frag, source.comp
		);
		return hash;
	}
};
}

namespace RE {

using ShaderProgramPtr = std::shared_ptr<const ShaderProgram>;

class ShaderProgramCache {
public:
	ShaderProgramCache() {}
	~ShaderProgramCache() {}

	/**
	* @brief Gets shader program as a shared resource.
	* @param sources Source codes of the shader program
	* @return Shader program as a shared resource
	*/
	ShaderProgramPtr getShaderProgram(const ShaderProgramSources& sources);
private:
	std::unordered_map<ShaderProgramSources, std::weak_ptr<ShaderProgram>> m_shaderMap;
};

}