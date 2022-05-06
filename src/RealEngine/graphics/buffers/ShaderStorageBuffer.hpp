/*! 
 *  \author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/graphics/buffers/InterfaceBlockBuffer.hpp>

namespace RE {

/**
 * @brief Allows arbitrary storage and retrieval of data from within shaders.
 *
 * Shader storage buffers are a type of interface block buffers.
*/
using ShaderStorageBuffer = InterfaceBlockBuffer<BufferType::SHADER_STORAGE>;

}