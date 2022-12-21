#version 460
#include <RealEngine/rendering/batches/shaders/sprite.glsl>

layout(vertices = 1) out;
layout(location = 0) patch out Sprite o_sprite;

#extension GL_EXT_scalar_block_layout : require
layout(std430, binding = 0) uniform PerSprite {
    Sprite u_sprites[1024];
};

void main() {
    gl_TessLevelOuter[0] = 1.0f;
    gl_TessLevelOuter[1] = 1.0f;
    gl_TessLevelOuter[2] = 1.0f;
    gl_TessLevelOuter[3] = 1.0f;
    gl_TessLevelInner[0] = 0.0f;
    gl_TessLevelInner[1] = 0.0f;
    o_sprite = u_sprites[gl_PrimitiveID];
}
