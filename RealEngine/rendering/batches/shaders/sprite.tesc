/*!
 *  @author    Dubsky Tomas
 */
#version 460
layout(vertices = 1) out;
layout(location = 0) patch out  vec4 o_pos;
layout(location = 1) patch out  vec4 o_uvs;
layout(location = 2) patch out  uint o_tex;
layout(location = 3) patch out  uint o_col;

layout(location = 0) in         vec4 i_pos[];
layout(location = 1) in         vec4 i_uvs[];
layout(location = 2) in         uint i_tex[];
layout(location = 3) in         uint i_col[];

void main() {
    gl_TessLevelOuter[0] = 1.0f;
    gl_TessLevelOuter[1] = 1.0f;
    gl_TessLevelOuter[2] = 1.0f;
    gl_TessLevelOuter[3] = 1.0f;
    gl_TessLevelInner[0] = 0.0f;
    gl_TessLevelInner[1] = 0.0f;

    o_pos = i_pos[gl_InvocationID];
    o_uvs = i_uvs[gl_InvocationID];
    o_tex = i_tex[gl_InvocationID];
    o_col = i_col[gl_InvocationID];
}
