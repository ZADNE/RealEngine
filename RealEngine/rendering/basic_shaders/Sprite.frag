#version 460
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D baseTexture;

void main() {
    color = fragColor * texture(baseTexture, fragUV);
}
