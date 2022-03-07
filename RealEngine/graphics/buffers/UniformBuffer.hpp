#pragma once

#include <RealEngine/graphics/buffers/InterfaceBlockBuffer.hpp>

namespace RE {

/**
 * @brief Allows sharing of uniforms among shader programs.
 *
 * Uniforms buffers are a type of interface block buffers.
*/
using UniformBuffer = InterfaceBlockBuffer<BufferType::UNIFORM>;

}