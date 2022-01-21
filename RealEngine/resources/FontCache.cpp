#include <RealEngine/resources/FontCache.hpp>

#include <iostream>
#include <algorithm>
#include <string>


namespace RE {

	FontCache::FontCache() {

	}

	FontCache::~FontCache() {
		for (auto& it : m_fontMap) {
			it.second.dispose();
		}
	}

	const Font* FontCache::getFont(const FontSeed& fontSeed) {
		auto mapIterator = m_fontMap.find(fontSeed);

		if (mapIterator == m_fontMap.end()) {//Font not loaded yet, loading it now
			Uint32 timer = SDL_GetTicks();
			m_fontMap.insert(std::make_pair(fontSeed, Font{ (m_fontFolder + "/" + fontSeed.name() + ".ttf").c_str(), fontSeed.size(), fontSeed.getStartChar(), fontSeed.getEndChar() }));
			std::cout << "Loaded font " << fontSeed.name() << " (" + std::to_string(fontSeed.size()) + " pts) in " << SDL_GetTicks() - timer << " ms.\n";
			return &m_fontMap[fontSeed];
		}

		//If the loaded font does not contain all needed characters
		if (fontSeed.getStartChar() < mapIterator->second.getStartChar() || fontSeed.getEndChar() > mapIterator->second.getEndChar()) {
			//Disposing previous load
			mapIterator->second.dispose();
			//Loading again with all required characters
			mapIterator->second.init((m_fontFolder + "/" + fontSeed.name() + ".ttf").c_str(), fontSeed.size(), std::min(fontSeed.getStartChar(), mapIterator->second.getStartChar()), std::max(fontSeed.getEndChar(), mapIterator->second.getEndChar()));
		}

		return &(mapIterator->second);
	}

	void FontCache::removeFont(const FontSeed& fontSeed) {
		auto mapIterator = m_fontMap.find(fontSeed);

		if (mapIterator != m_fontMap.end()) {//Font exists
			mapIterator->second.dispose();
			std::cout << "Deallocated font" << fontSeed.name() << " (" + std::to_string(fontSeed.size()) + " pts).\n";
		}
		m_fontMap.erase(fontSeed);
	}

}