#pragma once
#include <unordered_map>
#include <memory>

#include <RealEngine/ShaderProgram.hpp>


namespace RE {

	using ShaderProgramPtr = std::shared_ptr<const ShaderProgram>;

	struct ShaderProgramSeed {
		ShaderProgramSeed() :
			instance(0) {

		};

		ShaderProgramSeed(const ShaderProgramSource& sourcE, int instancE = 0) :
			source(sourcE),
			instance(instancE) {

		};

		bool operator==(const ShaderProgramSeed& other) const {
			return (source == other.source && instance == other.instance);
		}


		ShaderProgramSource source;
		int instance = 0;//Used for forcing multiple instances of the same shader

		static ShaderProgramSeed stdSprite;
		static ShaderProgramSeed stdGeometry;
	};

}

inline void hash_combine(std::size_t& seed) { }
template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	hash_combine(seed, rest...);
}

namespace std {
	template <> struct hash<RE::ShaderProgramSeed> {
		size_t operator()(const RE::ShaderProgramSeed& sps) const {
			std::size_t hash = 0;
			hash_combine(hash,
				sps.source.vert, sps.source.tesc, sps.source.tese,
				sps.source.geom, sps.source.frag, sps.source.comp,
				sps.instance);
			return hash;
		}
	};
}

namespace RE {

	class ShaderProgramCache {
	public:
		ShaderProgramCache() {}
		~ShaderProgramCache() {}

		ShaderProgramPtr getShaderProgram(const ShaderProgramSeed& shaderSeed);
	private:
		std::unordered_map<ShaderProgramSeed, std::weak_ptr<ShaderProgram>> m_shaderMap;
	};

}