/*!
 *  @author    Dubsky Tomas
 */
#include <algorithm>
#include <fstream>
#include <queue>
#include <set>
#include <spirv_glsl.hpp>

#include <RealShadersGenTool/ReflectionAnalysis.hpp>
#include <RealShadersGenTool/Utility.hpp>

namespace rsg {

constexpr int k_iBTypeCount{spirv_cross::SPIRType::BaseType::Struct + 1};

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
    BaseType::Double,  // Double
    BaseType::Struct,  // Struct
};

BaseType toBasicType(spirv_cross::SPIRType::BaseType type) {
    if (type >= k_typeConversionTable.size()) {
        return BaseType::Unknown;
    }
    return k_typeConversionTable[std::to_underlying(type)];
}

struct ReflectionResult {
    Struct strct;
    std::vector<spirv_cross::TypeID> requiredStructs;
};

ReflectionResult reflectStruct(
    const spirv_cross::Compiler& compiler, const spirv_cross::TypeID& id
) {
    ReflectionResult res;
    const auto& resType    = compiler.get_type(id);
    auto baseTypeId        = compiler.get_type(id).self;
    res.strct.name         = compiler.get_name(baseTypeId);
    res.strct.declaredSize = compiler.get_declared_struct_size(resType);

    // Reflect all members
    res.strct.members.resize(resType.member_types.size());
    for (size_t i = 0; i < resType.member_types.size(); ++i) {
        auto& reflMember       = res.strct.members[i];
        const auto& memberType = compiler.get_type(resType.member_types[i]);
        reflMember.name        = compiler.get_member_name(baseTypeId, i);
        reflMember.baseType    = toBasicType(memberType.basetype);
        switch (reflMember.baseType) {
        case BaseType::Unknown:
            fatalError("{}::{} has unsupported type", res.strct.name, reflMember.name);
        case BaseType::Struct:
            res.requiredStructs.emplace_back(memberType.self);
            reflMember.structTypeName = compiler.get_name(memberType.self);
            break;
        default: break;
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

    return res;
}

constexpr std::array<const char*, k_interfaceBlockTypeCount>
    k_interfaceBlockTypeNames{"uniform", "storage", "push constant range"};

const std::string& fetchResBlockName(
    const spirv_cross::Compiler& compiler, const spirv_cross::Resource& res
) {
    auto baseTypeId = compiler.get_type(res.base_type_id).self;
    return compiler.get_name(baseTypeId);
}

InterfaceBlockReflection reflectInterfaceBlock(
    std::span<const uint32_t> spirv, InterfaceBlockType blockType,
    std::string_view blockName
) {
    // Check whether the expected interface block is the only one present
    spirv_cross::Compiler compiler{spirv.data(), spirv.size()};
    auto resources = compiler.get_shader_resources();
    std::array<const spirv_cross::SmallVector<spirv_cross::Resource>*, k_interfaceBlockTypeCount>
        reses{
            &resources.uniform_buffers, &resources.storage_buffers,
            &resources.push_constant_buffers
        };
    int expected = static_cast<int>(blockType);
    for (size_t i = 0; i < k_interfaceBlockTypeCount; ++i) {
        if (i != expected && reses[i]->size() > 0) {
            fatalError(
                "Unexpected {} block '{}' - expecting only {}",
                k_interfaceBlockTypeNames[i],
                fetchResBlockName(compiler, (*reses[i])[0]), blockName
            );
        }
    }
    if (reses[expected]->size() > 1) {
        std::string_view bad{};
        for (const auto& res : (*reses[expected])) {
            const std::string& resBlockName = fetchResBlockName(compiler, res);
            if (resBlockName != blockName) {
                bad = resBlockName;
                break;
            }
        }
        fatalError(
            "Unexpected {} block '{}' - expecting only {}",
            k_interfaceBlockTypeNames[expected], bad, blockName
        );
    } else if (reses[expected]->size() == 0) {
        fatalError(
            "The expected {} block '{}' was not found",
            k_interfaceBlockTypeNames[expected], blockName
        );
    } else { // reses[expected]->size() == 1
        const auto& res                 = (*reses[expected])[0];
        const std::string& resBlockName = fetchResBlockName(compiler, res);
        if (resBlockName != blockName) {
            fatalError(
                "Unexpected {} block '{}' - expecting only {}",
                k_interfaceBlockTypeNames[expected], resBlockName, blockName
            );
        }
    }

    // Reflect the expected interface block
    InterfaceBlockReflection rval;
    std::set<spirv_cross::TypeID> reflectedStructs;
    std::queue<spirv_cross::TypeID> structsToReflect;
    auto enqueue = [&](spirv_cross::TypeID id) {
        if (!reflectedStructs.contains(id)) {
            structsToReflect.push(id);
            reflectedStructs.insert(id);
        }
    };
    enqueue((*reses[expected])[0].type_id);
    while (!structsToReflect.empty()) {
        ReflectionResult res = reflectStruct(compiler, structsToReflect.front());
        rval.structs.emplace_back(std::move(res.strct));
        structsToReflect.pop();
        for (auto it = res.requiredStructs.rbegin();
             it != res.requiredStructs.rend(); ++it) {
            enqueue(*it);
        }
    }
    std::reverse(rval.structs.begin(), rval.structs.end());
    return rval;
}

} // namespace rsg
