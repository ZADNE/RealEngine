/*!
 *  @author    Dubsky Tomas
 */
#include <fstream>
#include <print>
#include <spirv_glsl.hpp>

#include <RealShadersGenerator/CppGenerator.hpp>

namespace rsg {

std::string rsg::generateCppReflection(std::span<const uint32_t> spirv) {
    spirv_cross::Compiler compiler{spirv.data(), spirv.size()};
    /*auto reflectResources =
        [&](const spirv_cross::SmallVector<spirv_cross::Resource>& resources) {
            for (const auto& res : resources) {
                const auto& type      = compiler.get_type(res.type_id);
                const auto& base_type = compiler.get_type(res.base_type_id);

                size_t size = compiler.get_declared_struct_size(
                    compiler.get_type(res.base_type_id)
                );
                std::println("{}:{}", res.name, type.member_types.size());
                uint32_t count{};
                if (!type.array.empty()) {            // If it is arrayarray
                    if (type.array_size_literal[0]) { // If size is literal
                        count = type.array[0];
                    } else { // Size is defined by specialization constant
                        count = compiler.get_constant(type.array[0]).m.c[0].r->u32;
                    }
                } else {
                    count = 1;
                }
            }
        };
    auto resources = compiler.get_shader_resources();
    reflectResources(resources.uniform_buffers);
    reflectResources(resources.storage_buffers);*/

    return std::string();
}

} // namespace rsg
