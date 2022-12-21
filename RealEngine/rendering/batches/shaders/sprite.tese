#version 460
#include <RealEngine/rendering/batches/shaders/sprite.glsl>

layout(location = 0) out        vec2 o_uvs;
layout(location = 1) out flat   uint o_tex;
layout(location = 2) out flat   uint o_col;

layout(quads, equal_spacing) in;
layout(location = 0) patch in Sprite i_sprite;

layout(std430, push_constant) uniform PushConstants {
    mat4 u_projMat;
};

void main() {
    gl_Position = u_projMat * vec4(i_sprite.pos.xy + i_sprite.pos.zw * gl_TessCoord.xy, 0.0, 1.0);
    o_uvs = i_sprite.uvs.xy + i_sprite.uvs.zw * gl_TessCoord.xy;
    o_tex = i_sprite.tex;
    o_col = i_sprite.col;
}
