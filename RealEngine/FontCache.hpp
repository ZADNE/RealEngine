#pragma once
#include <unordered_map>

#include <RealEngine/Font.hpp>


namespace RE {

	class FontSeed {
	public:
		FontSeed() {};

		FontSeed(const std::string& name, int size, Uint16 cs = FIRST_PRINTABLE_CHAR, Uint16 ce = LAST_PRINTABLE_CHAR) : m_name(name), m_size(size), m_cs(cs), m_ce(ce) {

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
		Uint16 getStartChar() const { return m_cs; }
		Uint16 getEndChar() const { return m_ce; }
	private:
		std::string m_name;
		int m_size = 0;
		Uint16 m_cs = 0;
		Uint16 m_ce = 0;
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