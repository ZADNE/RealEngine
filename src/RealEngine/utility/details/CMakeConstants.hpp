/**
 *  @author    Dubsky Tomas
 *  @file
 *  @brief  Declares constants used by both CMake and runtime (eg. for hot reload)
 */
#pragma once

namespace re::details {

/**
 * @brief Name suffix used by shader-compilation targets
 */
constexpr const char* k_shaderTargetSuffix = "_Shaders";

/**
 * @brief File extension of C-array representation of SPIR-V of a shader
 */
constexpr const char* k_shaderSPIRVCFileExt = "spv.c";

/**
 * @brief File extension of dependency file of a shader
 */
constexpr const char* k_shaderDepFileExt = "d";

/**
 * @brief File extension of text disassembly of SPIR-V of a shader
 */
constexpr const char* k_shaderSPIRVDisFileExt = "spv.txt";

/**
 * @brief File extension of binary representation of SPIR-V of a shader
 */
constexpr const char* k_shaderSPIRVBinFileExt = "spv.bin";

} // namespace re::details
