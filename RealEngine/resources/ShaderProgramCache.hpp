#pragma once
#include <unordered_map>
#include <memory>

#include <RealEngine/graphics/ShaderProgram.hpp>

inline void hash_combine(std::size_t& seed) { }
template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	hash_combine(seed, rest...);
}

namespace std {
template <> struct hash<RE::ShaderProgramSource> {
	size_t operator()(const RE::ShaderProgramSource& source) const {
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
	* @param source Source code of the shader program
	* @return Shader program as a shared resource
	*/
	ShaderProgramPtr getShaderProgram(const ShaderProgramSource& source);
private:
	std::unordered_map<ShaderProgramSource, std::weak_ptr<ShaderProgram>> m_shaderMap;
};

}