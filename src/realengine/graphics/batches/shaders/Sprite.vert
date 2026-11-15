/**
 *  @author    Dubsky Tomas
 */
#version 460
layout(location = 0) out        vec4 o_pos;
layout(location = 1) out        vec4 o_uvs;
layout(location = 2) out flat   uint o_tex;
layout(location = 3) out flat   uint o_col;

layout(location = 0) in         vec4 i_pos;
layout(location = 1) in         vec4 i_uvs;
layout(location = 2) in         uint i_tex;
layout(location = 3) in         uint i_col;

void main() {
    o_pos = i_pos;
    o_uvs = i_uvs;
    o_tex = i_tex;
    o_col = i_col;
}
