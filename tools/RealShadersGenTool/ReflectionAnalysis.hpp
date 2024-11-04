/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cstdint>
#include <span>

#include <RealShadersGenTool/Common.hpp>
#include <RealShadersGenTool/InterfaceBlockReflection.hpp>

namespace rsg {

InterfaceBlockReflection reflectInterfaceBlock(
    std::span<const uint32_t> spirv, InterfaceBlockType blockType,
    std::string_view blockName
);

} // namespace rsg
