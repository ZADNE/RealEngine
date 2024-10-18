/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace rsg {

std::vector<uint32_t> compileToSpirV(
    const std::string& inputFile, const std::vector<std::string>& includeDirs
);

} // namespace rsg
