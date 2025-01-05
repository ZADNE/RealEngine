﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <vector>

namespace re::dp {

struct CLIArguments {
    std::vector<std::string> inputDirs;
    std::string outputDir;
    std::string indexFilepath;
};

CLIArguments parseArguments(int argc, char* argv[]);

} // namespace re::dp
