/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <vector>

namespace re::dp {

struct CLIArguments {
    std::vector<std::string> inputDirs;
    std::string outputDir;
};

CLIArguments parseArguments(int argc, char* argv[]);

} // namespace re::dp
