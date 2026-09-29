#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
} pc;

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform DraggerParam {
    vec3 color;
    float _p1;
} dg;

layout(location = 0) out vec3 vertexColor;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position             = (pc.projection * pc.modelview) * vec4(inPosition, 1.0);
    vertexColor             = dg.color;
}
