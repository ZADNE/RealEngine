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
 * @brief Is a texture that can be shared across Rooms
 */
using SharedTextureShaped = std::shared_ptr<TextureShaped>;

/**
 * @brief Is a data resource that can be shared across Rooms
 */
using SharedDataResource = std::shared_ptr<DataResource>;

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
    std::unordered_map<ResourceID::IDType, WeakResource> m_resourceMap;
};

} // namespace re
