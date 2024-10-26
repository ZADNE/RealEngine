/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <filesystem>

#include <RealShadersGenTool/InterfaceBlockReflection.hpp>

namespace rsg {

void generateCppFile(
    const InterfaceBlockReflection& reflection, const std::string& namespace_,
    const std::filesystem::path& outFile
);

} // namespace rsg
