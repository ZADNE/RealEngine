#pragma once
#include <unordered_map>

#include <RealEngine/graphics/Font.hpp>


namespace RE {

	class FontSeed {
	public:
		FontSeed() {};

		FontSeed(const std::string& name, int size, char32_t cs = FIRST_PRINTABLE_CHAR, char32_t ce = LAST_PRINTABLE_CHAR) : m_name(name), m_size(size), m_cs(cs), m_ce(ce) {

		}

		~FontSeed() {

		}

		bool operator==(const FontSeed& right) const {
			if (m_name == right.m_name && m_size == right.m_size) {
				return true;
			}
			return false;
		}

		std::string name() const { return m_name; }
		int size() const { return m_size; }
		char32_t getStartChar() const { return m_cs; }
		char32_t getEndChar() const { return m_ce; }
	private:
		std::string m_name;
		int m_size = 0;
		char32_t m_cs = 0;
		char32_t m_ce = 0;
	};

}

namespace std {
	template <> struct hash<RE::FontSeed> {
		size_t operator()(const RE::FontSeed& fs) const {
			return ((std::hash<std::string>()(fs.name()) << 1) + fs.size());
		}
	};
}

namespace RE {

	class FontCache {
	public:
		FontCache();
		~FontCache();

		const Font* getFont(const FontSeed& fontSeed);
		void removeFont(const FontSeed& fontSeed);
	private:
		std::unordered_map<FontSeed, Font> m_fontMap;
		std::string m_fontFolder = "fonts";
	};

}