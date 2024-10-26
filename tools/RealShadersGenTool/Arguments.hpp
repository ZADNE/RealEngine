/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <vector>

namespace rsg {

struct CLIArguments {
    std::string inputFile;
    std::string outputFile;
    std::vector<std::string> includeDirs;
    std::string namespace_;
};

CLIArguments parseArguments(int argc, char* argv[]);

} // namespace rsg
