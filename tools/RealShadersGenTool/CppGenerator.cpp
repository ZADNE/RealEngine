/*!
 *  @author    Dubsky Tomas
 */
#include <array>
#include <fstream>
#include <print>
#include <string_view>

#include <RealShadersGenTool/CppGenerator.hpp>
#include <RealShadersGenTool/Utility.hpp>

namespace std {
namespace fs = filesystem;
}

namespace rsg {

using namespace std::string_literals;

/**
 * @brief Index 0: name for scalar, Index 1: prefix for vector and matrix types
 */
using BaseTypeNames = std::array<std::string_view, 2>;

/**
 * @brief Ordered the same as spirv_cross::SPIRType::BaseType
 */
constexpr std::array<BaseTypeNames, std::to_underlying(BaseType::Count)> k_typeNames{
    BaseTypeNames{"bool", "b"},          // Boolean
    BaseTypeNames{"glm::int8", "i8"},    // SByte
    BaseTypeNames{"glm::uint8", "u8"},   // UByte
    BaseTypeNames{"glm::int16", "i16"},  // Short
    BaseTypeNames{"glm::uint16", "u16"}, // UShort
    BaseTypeNames{"glm::int32", "i32"},  // Int
    BaseTypeNames{"glm::uint32", "u32"}, // UInt
    BaseTypeNames{"glm::int64", "i64"},  // Int64
    BaseTypeNames{"glm::uint64", "u64"}, // UInt64
    BaseTypeNames{"glm::half", "h"},     // Half
    BaseTypeNames{"float", ""},          // Float
    BaseTypeNames{"double", "d"},        // Double
    BaseTypeNames{}                      // Unknown
};

constexpr std::array<std::string_view, 3> k_tensorInfixes{"", "vec", "mat"};

std::string basicTypeToString(const Member& member) {
    if (member.baseType >= BaseType::Count) {
        return ""s;
    }
    const auto& types = k_typeNames[std::to_underlying(member.baseType)];
    bool isVector     = member.vecSize >= 2;
    std::string rval{isVector ? "glm::"s : ""s};
    rval += types[isVector];
    bool isMatrix = member.columns >= 2;
    rval += k_tensorInfixes[isVector + isMatrix];
    rval += isVector ? std::to_string(isMatrix ? member.columns : member.vecSize)
                     : ""s;
    rval += isMatrix ? std::format("x{}", member.vecSize) : ""s;
    return rval;
}

constexpr std::string_view k_allowZeroSizedArrayBegin{
    "#pragma warning(push)\n"
    "#pragma warning(disable : 4200)\n"
};

constexpr std::string_view k_allowZeroSizedArrayEnd{"#pragma warning(pop)\n"};

std::string generateStruct(const Struct& strct) {
    std::string membersStr;
    bool runtimeSized = false;

    for (const Member& member : strct.members) {
        membersStr += "    ";
        std::string memberTypeStr = basicTypeToString(member);
        if (memberTypeStr == "") {
            fatalError("{}::{} has unsupported type", strct.name, member.name);
        }
        membersStr += memberTypeStr + ' ' + member.name;
        for (const auto& arraySize : member.arraySizes) {
            if (arraySize != 0) {
                membersStr += std::format("[{}]", arraySize);
            } else {
                membersStr += "[]";
                runtimeSized = true;
            }
        }
        membersStr += "{};\n";
    }

    std::string assertSize = std::format(
        "static_assert(sizeof({}) == {});", strct.name, strct.declaredSize
    );

    return std::format(
        "{}struct {} {{\n"
        "{}}};\n"
        "{}"
        "{}",
        runtimeSized ? k_allowZeroSizedArrayBegin : "", strct.name, membersStr,
        runtimeSized ? k_allowZeroSizedArrayEnd : "", assertSize
    );
}

std::string generateCppString(
    const InterfaceBlockReflection& reflection, const std::string& namespace_
) {
    std::string body;
    for (const auto& strct : reflection.structs) {
        body += generateStruct(strct);
    }

    std::string namespaceBegin =
        namespace_.empty() ? "" : std::format("namespace {} {{\n", namespace_);
    std::string namespaceEnd =
        namespace_.empty() ? "" : std::format("\n}} // namespace {}", namespace_);

    return std::format(
        "// Automatically generated file by RealShadersGenTool\n"
        "#pragma once\n"
        "\n"
        "{}\n"
        "{}\n"
        "{}\n",
        namespaceBegin, body, namespaceEnd
    );
}

void generateCppFile(
    const InterfaceBlockReflection& reflection, const std::string& namespace_,
    const std::filesystem::path& outFile
) {
    std::string str = generateCppString(reflection, namespace_);

    if (!std::fs::exists(outFile) || readWholeFile(outFile) != str) {
        std::ofstream out{outFile};
        out.write(str.c_str(), str.size());
    }
}

} // namespace rsg
