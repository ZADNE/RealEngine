/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace rsg {

constexpr int k_interfaceBlockTypeCount = 3;
enum class InterfaceBlockType {
    Uniform      = 0,
    Storage      = 1,
    PushConstant = 2
};

} // namespace rsg
