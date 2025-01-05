/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cstdint>

#include <RealEngine/utility/DebugName.hpp>

namespace re {

/**
 * @brief Is a unique identifier of a resource
 * @details The class does not retain path to the resource in release builds so
 *          it is very lightweight then.
 * @note    This class is not intended to be created directly.
 *          It should be created via re::resourceID(...) function generated
 *          by the ResourcePackager tool.
 */
class ResourceID {
public:

    /**
     * @brief The native type used as ID
     */
    using IDType = uint32_t;

    constexpr ResourceID(uint32_t id, const char* path)
        : m_id{id}
        , m_path{path} {}

    uint32_t id() const { return m_id; }
    operator uint32_t() const { return id(); }

    /**
     * @return Path in debug builds, an empty string otherwise.
     */
    const char* path() { return m_path; }

private:
    uint32_t m_id;
    [[no_unique_address]] DebugName<> m_path;
};

} // namespace re
