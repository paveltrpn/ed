#version 450

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 3) in vec3 inNormal;

void main() {
    const float outlineWidth = 0.05f;

    // Push vertices along normal by outline width
    vec3 inflatedPos = inPosition + normalize(inNormal) * outlineWidth;

    gl_Position = (pc.projection * pc.modelview) * vec4(inflatedPos, 1.0);
}
