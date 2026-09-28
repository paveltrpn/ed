#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

// 4x4 Bayer matrix, normalized to (0, 1) via (v + 0.5) / 16.0.
const float bayer4[16] = float[16](
    0.5f / 16.0f,  8.5f / 16.0f,  2.5f / 16.0f, 10.5f / 16.0f,
   12.5f / 16.0f,  4.5f / 16.0f, 14.5f / 16.0f,  6.5f / 16.0f,
    3.5f / 16.0f, 11.5f / 16.0f,  1.5f / 16.0f,  9.5f / 16.0f,
   15.5f / 16.0f,  7.5f / 16.0f, 13.5f / 16.0f,  5.5f / 16.0f );

void main() {
    // Binary dither: screen-space 4x4 Bayer threshold at 50% coverage.
    // Blending is disabled for this pipeline, so off pixels use discard
    // instead of a dithered alpha.
    // Screen-space index keeps the pattern fixed to the screen, so it
    // does not swim with camera motion.
    ivec2 bayerIndex = ivec2( mod(gl_FragCoord.xy, 4.0) );
    float threshold = bayer4[ bayerIndex.y * 4 + bayerIndex.x ];
    if (threshold > 0.5f) {
        discard;
    }

    outColor = vec4(fragColor, 1.0);
}
