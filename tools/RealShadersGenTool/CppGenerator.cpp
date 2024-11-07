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
    BaseTypeNames{"bool", "b"},            // Boolean
    BaseTypeNames{"std::int8_t", "i8"},    // SByte
    BaseTypeNames{"std::uint8_t", "u8"},   // UByte
    BaseTypeNames{"std::int16_t", "i16"},  // Short
    BaseTypeNames{"std::uint16_t", "u16"}, // UShort
    BaseTypeNames{"std::int32_t", "i32"},  // Int
    BaseTypeNames{"std::uint32_t", "u32"}, // UInt
    BaseTypeNames{"std::int64_t", "i64"},  // Int64
    BaseTypeNames{"std::uint64_t", "u64"}, // UInt64
    BaseTypeNames{"std::uint16_t", "u16"}, // Half
    BaseTypeNames{"float", ""},            // Float
    BaseTypeNames{"double", "d"},          // Double
    BaseTypeNames{"", ""},                 // Struct
    BaseTypeNames{}                        // Unknown
};

struct BaseTypeLayout {
    size_t alignment{};
    size_t size{};
};

BaseTypeLayout typeLayout(
    const Member& member, const InterfaceBlockReflection& reflection
) {
    constexpr static std::array<BaseTypeLayout, std::to_underlying(BaseType::Count)> k_typeLayouts{
        BaseTypeLayout{alignof(bool), sizeof(bool)},         // Boolean
        BaseTypeLayout{alignof(int8_t), sizeof(int8_t)},     // SByte
        BaseTypeLayout{alignof(uint8_t), sizeof(uint8_t)},   // UByte
        BaseTypeLayout{alignof(int16_t), sizeof(int16_t)},   // Short
        BaseTypeLayout{alignof(uint16_t), sizeof(uint16_t)}, // UShort
        BaseTypeLayout{alignof(int32_t), sizeof(int32_t)},   // Int
        BaseTypeLayout{alignof(uint32_t), sizeof(uint32_t)}, // UInt
        BaseTypeLayout{alignof(int64_t), sizeof(int64_t)},   // Int64
        BaseTypeLayout{alignof(uint64_t), sizeof(uint64_t)}, // UInt64
        BaseTypeLayout{alignof(short), sizeof(short)},       // Half
        BaseTypeLayout{alignof(float), sizeof(float)},       // Float
        BaseTypeLayout{alignof(double), sizeof(double)},     // Double
        BaseTypeLayout{},                                    // Struct
        BaseTypeLayout{}                                     // Unknown
    };

    if (member.baseType < BaseType::Struct) {
        return k_typeLayouts[std::to_underlying(member.baseType)];
    } else if (member.baseType == BaseType::Struct) {
        auto it = std::find_if(
            reflection.structs.begin(), reflection.structs.end(),
            [&](const Struct& strct) {
                return strct.name == member.structTypeName;
            }
        );
        if (it == reflection.structs.end()) {
            fatalError("Unknown struct referenced: {}", member.structTypeName);
        }
        return BaseTypeLayout{1, it->declaredSize};
    } else {
        fatalError("Unknown base type: {}", std::to_underlying(member.baseType));
    }
}

struct RequiredHeaders {
    bool cStdInt{};
    bool stdArray{};
    std::array<bool, 3> glmVec{};
    std::array<std::array<bool, 3>, 3> glmMat{};
    bool reAlignedTypes{};
};

std::string typeToString(
    const Member& member, bool useStd140Vec, RequiredHeaders& reqHeaders
) {
    const auto& types = k_typeNames[std::to_underlying(member.baseType)];
    bool isVector     = member.vecSize >= 2;
    bool isMatrix     = member.columns >= 2;
    if (member.baseType == BaseType::Struct) {
        return member.structTypeName + (useStd140Vec ? "_140" : "");
    } else if (useStd140Vec) {
        reqHeaders.reAlignedTypes = true;
        return std::format("re::rs::{}vec{}_140", types[1], member.vecSize);
    } else if (isMatrix) {
        reqHeaders.glmMat[member.columns - 2][member.vecSize - 2] = true;
        return std::format("glm::{}mat{}x{}", types[1], member.columns, member.vecSize);
    } else if (isVector) {
        reqHeaders.glmVec[member.vecSize - 2] = true;
        return std::format("glm::{}vec{}", types[1], member.vecSize);
    } else {
        reqHeaders.cStdInt = true;
        return std::string{types[0]};
    }
}

constexpr std::string_view k_allowZeroSizedArrayBegin{
    "#pragma warning(push)\n"
    "#pragma warning(disable : 4200)\n"
};

constexpr std::string_view k_allowZeroSizedArrayEnd{"#pragma warning(pop)\n"};

std::string generateStruct(
    const Struct& strct, const InterfaceBlockReflection& reflection,
    RequiredHeaders& reqHeaders, bool isTopLevel
) {
    size_t offset = 0;
    std::string membersStr;
    bool runtimeSized = false;

    for (Member member : strct.members) {
        // Check if type is valid
        if (member.baseType >= BaseType::Count) {
            fatalError("{}::{} has unsupported type", strct.name, member.name);
        }

        // Stride adjustment
        BaseTypeLayout layout = typeLayout(member, reflection);
        size_t elemSize       = layout.size * member.vecSize * member.columns;
        bool useStd140Vec     = false;
        if (elemSize != member.elemStrideBytes) {
            if (member.vecSize == 3 &&
                (layout.size * 4 * member.columns) == member.elemStrideBytes) {
                member.vecSize = 4;  // *vec3 to *vec4 or *mat*x3 to *mat*x4
            } else if (member.elemStrideBytes == 16) {
                useStd140Vec = true; // std140 array of scalars or vectors
            } else if (member.elemStrideBytes / member.columns == 16 &&
                (layout.size * 4 * member.columns) == member.elemStrideBytes) {
                member.vecSize = 4;  // std140 array of mat*x2
            } else {
                fatalError(
                    "{}::{} has unexpected stride {}", strct.name, member.name,
                    member.elemStrideBytes
                );
            }
            elemSize = member.elemStrideBytes;
        }

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
        std::string memberTypeStr = typeToString(member, useStd140Vec, reqHeaders);
        size_t elemCount = 1;
        for (const auto& arraySize : member.arraySizes) {
            if (arraySize != 0) {
                elemCount *= arraySize;
                memberTypeStr =
                    std::format("std::array<{}, {}>", memberTypeStr, arraySize);
                reqHeaders.stdArray = true;
            } else {
                runtimeSized = true;
            }
        }

        // Whole member declaration
        membersStr += std::format(
            "{} {}{};\n", memberTypeStr, member.name, runtimeSized ? "[]" : "{}"
        );
        offset += elemSize * elemCount;
    }

    std::string assertSize =
        runtimeSized
            ? std::format("static_assert(sizeof({}) == {});", strct.name, strct.declaredSize)
            : std::format(
                  "static_assert(offsetof({0}, {1}) + sizeof({0}::{1}) == "
                  "{2});",
                  strct.name, strct.members.back().name, strct.declaredSize
              );

    std::string alignedType =
        isTopLevel
            ? ""s
            : std::format(
                  "struct alignas(16) {0}_140: public {0} {{}};\n", strct.name
              );

    return std::format(
        "{}struct {} {{\n"
        "{}}};\n"
        "{}"
        "{}\n"
        "{}\n",
        runtimeSized ? k_allowZeroSizedArrayBegin : "", strct.name, membersStr,
        runtimeSized ? k_allowZeroSizedArrayEnd : "", assertSize, alignedType
    );
}

std::string generateCppString(
    const InterfaceBlockReflection& reflection, const std::string& namespace_
) {
    // Compose structs
    RequiredHeaders reqHeaders;
    std::string body;
    {
        auto it = reflection.structs.begin();
        for (; (it + 1) != reflection.structs.end(); ++it) {
            body += generateStruct(*it, reflection, reqHeaders, false);
        }
        body += generateStruct(*it, reflection, reqHeaders, true);
    }

    // Compose includes
    std::string includeGroup = "#include <cstddef>\n";
    std::string includes;
    auto endIncludeGroup = [&]() {
        if (!includeGroup.empty()) {
            includes += includeGroup + '\n';
            includeGroup.clear();
        }
    };
    if (reqHeaders.cStdInt) {
        includeGroup += "#include <cstdint>\n";
    }
    if (reqHeaders.stdArray) {
        includeGroup += "#include <array>\n";
    }
    endIncludeGroup();
    for (size_t i = 0; i < reqHeaders.glmVec.size(); i++) {
        if (reqHeaders.glmVec[i]) {
            includeGroup += std::format("#include <glm/vec{}.hpp>\n", i + 2);
        }
    }
    for (size_t cols = 0; cols < reqHeaders.glmMat.size(); cols++) {
        for (size_t rows = 0; rows < reqHeaders.glmMat[0].size(); rows++) {
            if (reqHeaders.glmMat[cols][rows]) {
                includeGroup += std::format(
                    "#include <glm/mat{}x{}.hpp>\n", cols + 2, rows + 2
                );
            }
        }
    }
    endIncludeGroup();
    if (reqHeaders.reAlignedTypes) {
        includeGroup += "#include <RealShaders/Std140AlignedTypes.hpp>\n";
    }
    endIncludeGroup();

    // Compose namespace
    std::string namespaceBegin =
        namespace_.empty() ? ""s : std::format("namespace {} {{\n", namespace_);
    std::string namespaceEnd =
        namespace_.empty() ? ""s : std::format("\n}} // namespace {}", namespace_);

    // Compose whole file
    return std::format(
        "// Automatically generated file by RealShadersGenTool\n"
        "#pragma once\n"
        "{}"
        "{}\n"
        "{}\n"
        "{}\n",
        includes, namespaceBegin, body, namespaceEnd
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
