/*!
 *  @author    Dubsky Tomas
 */
#include <fstream>
#include <spirv_glsl.hpp>

#include <RealShadersGenTool/CppGenerator.hpp>
#include <RealShadersGenTool/Utility.hpp>

namespace rsg {

constexpr int k_iBTypeCount{spirv_cross::SPIRType::BaseType::Struct};

constexpr std::array<BaseType, k_iBTypeCount> k_typeConversionTable{
    BaseType::Unknown, // Unknown
    BaseType::Unknown, // Void
    BaseType::Boolean, // Boolean
    BaseType::SByte,   // SByte
    BaseType::UByte,   // UByte
    BaseType::Short,   // Short
    BaseType::UShort,  // UShort
    BaseType::Int,     // Int
    BaseType::UInt,    // UInt
    BaseType::Int64,   // Int64
    BaseType::UInt64,  // UInt64
    BaseType::Unknown, // AtomicCounter
    BaseType::Half,    // Half
    BaseType::Float,   // Float
    BaseType::Double   // Double
};

BaseType toBasicType(spirv_cross::SPIRType::BaseType type) {
    if (type >= k_typeConversionTable.size()) {
        return BaseType::Unknown;
    }
    return k_typeConversionTable[std::to_underlying(type)];
}

InterfaceBlockReflection generateCppReflectionOfResource(
    const spirv_cross::Compiler& compiler, const spirv_cross::Resource& res
) {
    InterfaceBlockReflection rval;
    Struct& ib          = rval.structs.emplace_back();
    const auto& resType = compiler.get_type(res.type_id);
    ib.name             = res.name;
    ib.declaredSize     = compiler.get_declared_struct_size(resType);

    // Reflect all members
    ib.members.resize(resType.member_types.size());
    for (size_t i = 0; i < resType.member_types.size(); ++i) {
        auto& reflMember       = ib.members[i];
        const auto& memberType = compiler.get_type(resType.member_types[i]);
        reflMember.name        = compiler.get_member_name(res.base_type_id, i);
        reflMember.baseType    = toBasicType(memberType.basetype);
        if (reflMember.baseType == BaseType::Unknown) {
            fatalError("{}::{} has unsupported type", ib.name, reflMember.name);
        }
        reflMember.offsetBytes = compiler.type_struct_member_offset(resType, i);
        reflMember.sizeBytes = compiler.get_declared_struct_member_size(resType, i);
        reflMember.vecSize = memberType.vecsize;
        reflMember.columns = memberType.columns;
        reflMember.arraySizes.reserve(memberType.array.size());
        size_t elemCount = 1;
        for (size_t j = 0; j < memberType.array.size(); ++j) {
            size_t size = memberType.array[j];
            if (memberType.array_size_literal[j] &&
                size != 0) { // If size is literal
                elemCount *= size;
                reflMember.arraySizes.push_back(size);
            } else { // Size is defined by a spec constant or runtime
                reflMember.arraySizes.push_back(0);
            }
        }
        if (memberType.array.size() > 0) {
            size_t stride = compiler.type_struct_member_array_stride(resType, i);
            size_t lastSize            = reflMember.arraySizes.back();
            reflMember.elemStrideBytes = (stride * (lastSize ? lastSize : 1)) /
                                         elemCount;
        } else {
            reflMember.elemStrideBytes = reflMember.sizeBytes;
        }
    }

    return rval;
}

constexpr int k_interfaceBlockTypeCount = 3;
enum class InterfaceBlockType {
    Uniform      = 0,
    Storage      = 1,
    PushConstant = 2
};

constexpr std::array<const char*, k_interfaceBlockTypeCount>
    k_interfaceBlockTypeNames{"uniform", "storage", "push constant range"};

InterfaceBlockReflection reflectInterfaceBlock(
    std::span<const uint32_t> spirv, std::string_view expectedTypename
) {
    // Determine what type to expect
    InterfaceBlockType expectedType{};
    if (expectedTypename.size() <= 2) {
        fatalError(
            "Name of the interface block is '{}' which means it has no name "
            "beyond type suffix.",
            expectedTypename
        );
    }
    std::string_view suffix = expectedTypename.substr(expectedTypename.size() - 2);
    if (suffix == "UB") {
        expectedType = InterfaceBlockType::Uniform;
    } else if (suffix == "SB") {
        expectedType = InterfaceBlockType::Storage;
    } else if (suffix == "PC") {
        expectedType = InterfaceBlockType::PushConstant;
    } else {
        fatalError(
            "Unexpected interface block suffix '{}' -  expected either 'UB', "
            "'SB', or 'PC'.",
            suffix
        );
    }

    // Check whether the expected interface block is the only one present
    spirv_cross::Compiler compiler{spirv.data(), spirv.size()};
    auto resources = compiler.get_shader_resources();
    std::array<const spirv_cross::SmallVector<spirv_cross::Resource>*, k_interfaceBlockTypeCount>
        reses{
            &resources.uniform_buffers, &resources.storage_buffers,
            &resources.push_constant_buffers
        };
    int expected = static_cast<int>(expectedType);
    for (size_t i = 0; i < k_interfaceBlockTypeCount; ++i) {
        if (i != expected && reses[i]->size() > 0) {
            fatalError(
                "Unexpected {} block '{}' - expecting only {}",
                k_interfaceBlockTypeNames[i], (*reses[i])[0].name, expectedTypename
            );
        }
    }
    if (reses[expected]->size() > 1) {
        std::string_view bad{};
        for (const auto& res : (*reses[expected])) {
            if (res.name != expectedTypename) {
                bad = res.name;
                break;
            }
        }
        fatalError(
            "Unexpected {} block '{}' - expecting only {}",
            k_interfaceBlockTypeNames[expected], bad, expectedTypename
        );
    } else if (reses[expected]->size() == 0) {
        fatalError(
            "The expected {} block '{}' was not found",
            k_interfaceBlockTypeNames[expected], expectedTypename
        );
    } else { // reses[expected]->size() == 1
        const auto& res = (*reses[expected])[0];
        if (res.name != expectedTypename) {
            fatalError(
                "Unexpected {} block '{}' - expecting only {}",
                k_interfaceBlockTypeNames[expected], (*reses[expected])[0].name,
                expectedTypename
            );
        }
    }

    // Reflect the expected interface block
    return generateCppReflectionOfResource(compiler, (*reses[expected])[0]);
}

} // namespace rsg
