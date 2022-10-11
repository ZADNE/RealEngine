#version 460
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform GlobalMatrices {
    mat4 projectionMatrix;
};

void main() {
    gl_Position = projectionMatrix * vec4(position, 0.0, 1.0);
    fragColor = color;
}
