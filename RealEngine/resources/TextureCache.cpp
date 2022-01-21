#include <RealEngine/resources/TextureCache.hpp>

namespace RE {

TexturePtr TextureCache::getTexture(const std::string& filePathNoExt) {
	auto mapIterator = m_textureMap.find(filePathNoExt);
	TexturePtr stored;
	if (mapIterator != m_textureMap.end() && (stored = mapIterator->second.lock())) {
		return stored;
	} else {//Texture never accessed before or it has expired
		auto shared_p = std::make_shared<Texture>(m_textureFolder + filePathNoExt + ".png");
		m_textureMap.insert_or_assign(filePathNoExt, shared_p);
		return shared_p;
	}
}

}