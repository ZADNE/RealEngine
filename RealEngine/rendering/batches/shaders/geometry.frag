#version 460
layout(location = 0) out vec4 o_col;

layout(location = 0) in vec4 i_col;

void main() {
    o_col = i_col;
}
