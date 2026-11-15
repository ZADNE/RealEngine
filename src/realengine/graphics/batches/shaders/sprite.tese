/**
 *  @author    Dubsky Tomas
 */
#version 460
layout(location = 0) out        vec2 o_uvs;
layout(location = 1) out flat   uint o_tex;
layout(location = 2) out flat   uint o_col;

layout(quads, equal_spacing) in;
layout(location = 0) patch in   vec4 i_pos;
layout(location = 1) patch in   vec4 i_uvs;
layout(location = 2) patch in   uint i_tex;
layout(location = 3) patch in   uint i_col;

layout(std430, push_constant) uniform PushConstants {
    mat4 p_mvpMat;
};

void main() {
    gl_Position = p_mvpMat * vec4(i_pos.xy + i_pos.zw * gl_TessCoord.xy, 0.0, 1.0);
    o_uvs = i_uvs.xy + i_uvs.zw * gl_TessCoord.xy;
    o_uvs.y = 1.0 - o_uvs.y;// :-)
    o_tex = i_tex;
    o_col = i_col;
}
