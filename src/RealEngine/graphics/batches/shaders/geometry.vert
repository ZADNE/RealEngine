/**
 *  @author    Dubsky Tomas
 */
#version 460
layout(location = 0) out vec4 o_col;

layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec4 i_col;

layout(std430, push_constant) uniform PushConstants {
    mat4 p_mvpMat;
};

void main() {
    gl_Position = p_mvpMat * vec4(i_pos, 0.0, 1.0);
    o_col = i_col;
}
