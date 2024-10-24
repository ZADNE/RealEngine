/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <vector>

namespace rsg {

enum class BaseType {
    Boolean,
    SByte,
    UByte,
    Short,
    UShort,
    Int,
    UInt,
    Int64,
    UInt64,
    Half,
    Float,
    Double,
    Unknown,
    Count
};

struct Member {
    BaseType baseType;
    size_t vecSize{};
    size_t columns{};
    std::string name;
    std::vector<size_t> arraySizes;
};

struct Struct {
    std::string name;
    std::vector<Member> members;
    size_t declaredSize{};
};

struct InterfaceBlockReflection {
    std::vector<Struct> structs; ///< In order of how they should appear
};

} // namespace rsg
