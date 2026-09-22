#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 vertexColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 vLighting;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform ObjectParamsBuffer {
    int appearenceMode;
    int lightMode;
    int _p1;
    int _p2;
} op;

void main() {
    vec3 choosedColor = vec3(0.0, 0.0, 0.0);
    if (op.appearenceMode == 0) {
        // Object appearence mode == COLOR.
        choosedColor = vertexColor;
    } else if (op.appearenceMode == 1) {
        // Object appearence mode == TEXTURE.
        vec4 colorFromTexture = texture(texSampler, fragTexCoord);
        choosedColor = colorFromTexture.rgb;
    }

    vec3 choosedLight = vec3(1.0, 1.0, 1.0);
    if (op.lightMode == 0) {
        // Object light mode == NONE.
        choosedLight = vec3(1.0, 1.0, 1.0);
    } else if (op.lightMode == 1) {
        // Object light mode == CONSTANT.
        choosedLight = vLighting;
    } else if (op.lightMode == 2) {
        // Object light mode == INSCENE.
        vec4 colorFromTexture = texture(texSampler, fragTexCoord);
        choosedLight = vec3(0.0, 0.0, 0.0);
    }

    outColor = vec4(choosedColor*choosedLight, 1.0f);
}
