/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cstdint>
#include <span>

#include <RealShadersGenerator/InterfaceBlockReflection.hpp>

namespace rsg {

InterfaceBlockReflection reflectInterfaceBlock(
    std::span<const uint32_t> spirv, std::string_view expectedTypename
);

} // namespace rsg
