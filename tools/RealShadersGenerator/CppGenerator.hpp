/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cstdint>
#include <span>
#include <string>

namespace rsg {

std::string generateCppReflection(std::span<const uint32_t> spirv);

} // namespace rsg
