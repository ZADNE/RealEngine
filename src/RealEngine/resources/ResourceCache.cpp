/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceCache.hpp>

namespace re {

ResourceCache::ResourceCache(const ResourceLoader& resourceLoader)
    : m_resourceLoader{resourceLoader} {
}

template<typename T>
    requires IsResource<T>
std::shared_ptr<T> ResourceCache::resource(ResourceID id) {
    auto it = m_resourceMap.find(id);
    std::shared_ptr<T> stored;
    if (it != m_resourceMap.end() &&
        (stored = std::get<std::weak_ptr<T>>(it->second).lock())) {
        // Resource present
        return stored;
    } else {
        // Resource never accessed before or it has expired
        auto made = std::make_shared<T>(m_resourceLoader.load<T>(id));
        m_resourceMap.insert_or_assign(id, made);
        return made;
    }
}

template<typename T>
using Shrd = std::shared_ptr<T>;

template Shrd<DataResource> ResourceCache::resource<DataResource>(ResourceID id);
template Shrd<TextureShaped> ResourceCache::resource<TextureShaped>(ResourceID id);

} // namespace re
