/*!
 *  @author    Dubsky Tomas
 */
#include <array>
#include <fstream>
#include <print>
#include <string_view>

#include <glm/glm.hpp>

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

constexpr std::array<BaseTypeNames, std::to_underlying(BaseType::Count)> k_typeNames{
    BaseTypeNames{"bool", "b"},       // Boolean
    BaseTypeNames{"int8_t", "i8"},    // SByte
    BaseTypeNames{"uint8_t", "u8"},   // UByte
    BaseTypeNames{"int16_t", "i16"},  // Short
    BaseTypeNames{"uint16_t", "u16"}, // UShort
    BaseTypeNames{"int32_t", "i32"},  // Int
    BaseTypeNames{"uint32_t", "u32"}, // UInt
    BaseTypeNames{"int64_t", "i64"},  // Int64
    BaseTypeNames{"uint64_t", "u64"}, // UInt64
    BaseTypeNames{"uint16_t", "u16"}, // Half
    BaseTypeNames{"float", ""},       // Float
    BaseTypeNames{"double", "d"},     // Double
    BaseTypeNames{}                   // Unknown
};

struct BaseTypeLayout {
    int alignment{};
    int size{};
};

constexpr std::array<BaseTypeLayout, std::to_underlying(BaseType::Count)> k_typeLayouts{
    BaseTypeLayout{alignof(bool), sizeof(bool)},               // Boolean
    BaseTypeLayout{alignof(glm::int8), sizeof(glm::int8)},     // SByte
    BaseTypeLayout{alignof(glm::uint8), sizeof(glm::uint8)},   // UByte
    BaseTypeLayout{alignof(glm::int16), sizeof(glm::int16)},   // Short
    BaseTypeLayout{alignof(glm::uint16), sizeof(glm::uint16)}, // UShort
    BaseTypeLayout{alignof(glm::int32), sizeof(glm::int32)},   // Int
    BaseTypeLayout{alignof(glm::uint32), sizeof(glm::uint32)}, // UInt
    BaseTypeLayout{alignof(glm::int64), sizeof(glm::int64)},   // Int64
    BaseTypeLayout{alignof(glm::uint64), sizeof(glm::uint64)}, // UInt64
    BaseTypeLayout{alignof(short), sizeof(short)},             // Half
    BaseTypeLayout{alignof(float), sizeof(float)},             // Float
    BaseTypeLayout{alignof(double), sizeof(double)},           // Double
    BaseTypeLayout{}                                           // Unknown
};

constexpr std::array<std::string_view, 3> k_tensorInfixes{"", "vec", "mat"};

std::string basicTypeToString(const Member& member) {
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
    size_t offset = 0;
    std::string membersStr;
    bool runtimeSized = false;

    for (Member member : strct.members) {
        // Check if type is valid
        if (member.baseType >= BaseType::Count) {
            fatalError("{}::{} has unsupported type", strct.name, member.name);
        }

        // Fix up sizes
        BaseTypeLayout layout = k_typeLayouts[std::to_underlying(member.baseType)];
        member.vecSize = member.vecSize == 3 ? 4 : member.vecSize;
        size_t stride  = layout.size * member.vecSize * member.columns;
        if (member.arrStrideBytes > 0) {
            size_t columnCount = 1;
            if (member.arraySizes.size() > 1) {
                for (size_t i = 0; i < member.arraySizes.size() - 1; ++i) {
                    columnCount *= member.arraySizes[i] ? member.arraySizes[i] : 1;
                }
            }
            member.vecSize *= member.arrStrideBytes / (stride * columnCount);
        }
        stride = layout.size * member.vecSize * member.columns;

        // Alignment
        size_t forcedAlignment = layout.alignment;
        while (roundToMultiple(offset, forcedAlignment) < member.offsetBytes) {
            forcedAlignment *= 2;
        }
        offset = roundToMultiple(offset, forcedAlignment);
        membersStr += "    ";
        if (forcedAlignment > layout.alignment) {
            membersStr += std::format("alignas({}) ", forcedAlignment);
        }

        // Type with std::arrays
        std::string memberTypeStr = basicTypeToString(member);
        size_t elemCount          = 1;
        for (const auto& arraySize : member.arraySizes) {
            if (arraySize != 0) {
                elemCount *= arraySize;
                memberTypeStr =
                    std::format("std::array<{}, {}>", memberTypeStr, arraySize);
            } else {
                runtimeSized = true;
            }
        }

        // Whole member declaration
        membersStr += std::format(
            "{} {}{};\n", memberTypeStr, member.name, runtimeSized ? "[]" : "{}"
        );
        offset += stride * elemCount;
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
