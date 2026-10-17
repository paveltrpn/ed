#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertexColor;

// TODO: deal with "flat" modifier.
layout(location = 1) in flat int isSelected;

layout(location = 0) out vec4 outColor;

// 4x4 Bayer matrix, normalized to (0, 1) via (v + 0.5) / 16.0.
// Same pattern as vk_grid_FRAGMENT.glsl.
const float bayer4[16] = float[16](
    0.5f / 16.0f,  8.5f / 16.0f,  2.5f / 16.0f, 10.5f / 16.0f,
   12.5f / 16.0f,  4.5f / 16.0f, 14.5f / 16.0f,  6.5f / 16.0f,
    3.5f / 16.0f, 11.5f / 16.0f,  1.5f / 16.0f,  9.5f / 16.0f,
   15.5f / 16.0f,  7.5f / 16.0f, 13.5f / 16.0f,  5.5f / 16.0f );

// Constant screen-door opacity for the gizmo overlay.
const float opacity = 0.52f;

// Grain scale: each Bayer cell covers a 2x2 pixel block.
const float grainScale = 2.0f;

void main() {
    ivec2 bayerIndex = ivec2( mod(floor(gl_FragCoord.xy / grainScale), 4.0) );
    float threshold = bayer4[ bayerIndex.y * 4 + bayerIndex.x ];

    // Discard so culled pixels leave color and depth untouched,
    // letting the scene show through the holes.
    if ((threshold > opacity) && (isSelected == 0)) {
        discard;
    }

    outColor = vec4(vertexColor.rgb, 1.0f);
}
