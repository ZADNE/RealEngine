/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cstddef>

/**
 * @brief Calculates byte offset of element of array within a type
 * @param type A type which contains 'arrayMember' non-static member
 * @param arrayMember Name of the member within 'type'. Must be a C-array-like type.
 * @param index Index of the element within 'arrayMember'
 * @return Offset of type::arrayMember[index] in bytes
 */
#define RE_OFFSET_OF_ARR(type, arrayMember, index) \
    (offsetof(type, arrayMember) + sizeof(type::arrayMember[0]) * index)
