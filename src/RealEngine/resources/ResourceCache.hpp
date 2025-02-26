/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <memory>
#include <unordered_map>
#include <variant>

#include <RealEngine/resources/ResourceLoader.hpp>

namespace re {

/**
 * @brief Caches resources to avoid duplication
 */
class ResourceCache {
public:
    explicit ResourceCache(const ResourceLoader& resourceLoader);

    template<typename T>
        requires IsResource<T>
    std::shared_ptr<T> resource(ResourceID id);

private:
    const ResourceLoader& m_resourceLoader;

    using WeakResource =
        std::variant<std::weak_ptr<DataResource>, std::weak_ptr<TextureShaped>>;
    std::unordered_map<ResourceID, WeakResource> m_resourceMap;
};

} // namespace re
