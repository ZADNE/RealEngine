/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <span>
#include <string>

namespace re::dp {

void composePackage(
    std::span<const std::string> inputDirs, const std::string& outputDir,
    const std::string& indexFilepath
);

} // namespace re::dp
