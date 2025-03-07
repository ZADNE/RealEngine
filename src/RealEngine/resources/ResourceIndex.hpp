/**
 *  @author    Dubsky Tomas
 *  @file      ResourceIndex.hpp
 *  @brief     Is an empty placeholder that will be overriden by a real index.
 *  @details   This file is special. After running ResourcePackager at least
 *             once, a proper resource index will be generated in the build
 *             directory.
 *  @details   The real index will be generated at the same relative path so it
 *             will actually take precedense over this file.
 *  @details   So this file exists only to enable build of your application
 *             without running the ResourcePackager. That is useful for debug
 *             builds because those actually use unpackaged data.
 */
#pragma once
#include <RealEngine/resources/ResourceID.hpp>
#include <RealEngine/utility/CompTimeString.hpp>

namespace re {

/**
 * @brief Resolves a resource identified by string path into ID.
 * @details The ResourcePackager will generated a concrete specialization for
 *          each resource.
 */
template<CompTimeString k_lit>
consteval ResourceID resourceID() {
    if constexpr (k_buildType == BuildType::Debug) {
        // Allow non-indexed data in debug builds.
        // The debug-only path will be used to load the data.
        return ResourceID{~0u, static_cast<const char*>(k_lit.data)};
    } else {
        // All data must be indexed in release builds.
        // This is because paths are not preserved at all.
        throw "Trying to use a non-indexed resource.";
    }
};

} // namespace re
