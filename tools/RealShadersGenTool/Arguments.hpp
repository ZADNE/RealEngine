/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <vector>

#include <RealShadersGenTool/Common.hpp>

namespace rsg {

struct CLIArguments {
    InterfaceBlockType blockType{};
    std::string blockName;
    std::string inputFile;
    std::string outputFile;
    std::vector<std::string> includeDirs;
    std::string namespace_;
};

CLIArguments parseArguments(int argc, char* argv[]);

} // namespace rsg
