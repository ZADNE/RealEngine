/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <span>
#include <string>

namespace re::dp {

void composeTileItemTextures(
    std::span<const std::string> inputDirs, const std::string& outputDir
);

} // namespace re::dp
