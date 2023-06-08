/*!
 *  @author    Dubsky Tomas
 */
#version 460
layout(location = 0) in         vec2 i_uvs;
layout(location = 1) in flat    uint i_tex;
layout(location = 2) in flat    uint i_col;

layout(location = 0) out        vec4 o_col;

layout(constant_id = 0) const   uint k_maxTextures = 64;

#extension GL_EXT_nonuniform_qualifier : require
layout(binding = 0) uniform sampler2D u_textures[k_maxTextures];

void main() {
    o_col = unpackUnorm4x8(i_col) * texture(u_textures[nonuniformEXT(i_tex)], i_uvs);
}
