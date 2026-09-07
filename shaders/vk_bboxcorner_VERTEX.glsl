#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
} pc;

layout(set = 0, binding = 0) uniform BoxTransformMatrix {
    mat4 matrix;
} boxTrnasformToObject;

layout(location = 0) out vec3 fragColor;

out gl_PerVertex {
    vec4 gl_Position;
};

vec3 colors[8] = vec3[](
    vec3(1.0, 0.0, 0.0),     // 0: red
    vec3(0.0, 1.0, 0.0),     // 1: green
    vec3(0.0, 0.0, 1.0),     // 2: blue
    vec3(1.0, 1.0, 0.0),     // 3: yellow
    vec3(1.0, 0.0, 1.0),     // 4: magenta
    vec3(0.0, 1.0, 1.0),     // 5: cyan
    vec3(1.0, 0.5, 0.0),     // 6: orange
    vec3(0.5, 0.5, 0.5)      // 7: gray
);

const float LINE_LENGTH = 0.25;

mat4 getScaleMatrix(vec3 sc) {
    return mat4(
        sc.x, 0.0,  0.0,  0.0,  // Column 1
        0.0,  sc.y, 0.0,  0.0,  // Column 2
        0.0,  0.0,  sc.z, 0.0,  // Column 3
        0.0,  0.0,  0.0,  1.0   // Column 4
    );
}

vec3 lineVerts[48] = vec3[](
    // Corner 0 (-0.5,-0.5,-0.5) back-bottom-left - red
    vec3(-0.5, -0.5, -0.5), vec3(-0.5 + LINE_LENGTH, -0.5, -0.5),  // +X edge toward corner 1
    vec3(-0.5, -0.5, -0.5), vec3(-0.5, -0.5 + LINE_LENGTH, -0.5),  // +Y edge toward corner 3
    vec3(-0.5, -0.5, -0.5), vec3(-0.5, -0.5, -0.5 + LINE_LENGTH),  // +Z edge toward corner 4
    // Corner 1 (0.5,-0.5,-0.5) back-bottom-right - green
    vec3( 0.5, -0.5, -0.5), vec3( 0.5 - LINE_LENGTH, -0.5, -0.5),  // -X edge toward corner 0
    vec3( 0.5, -0.5, -0.5), vec3( 0.5, -0.5 + LINE_LENGTH, -0.5),  // +Y edge toward corner 2
    vec3( 0.5, -0.5, -0.5), vec3( 0.5, -0.5, -0.5 + LINE_LENGTH),  // +Z edge toward corner 5
    // Corner 2 (0.5,0.5,-0.5) back-top-right - blue
    vec3( 0.5,  0.5, -0.5), vec3( 0.5 - LINE_LENGTH,  0.5, -0.5),  // -X edge toward corner 3
    vec3( 0.5,  0.5, -0.5), vec3( 0.5,  0.5 - LINE_LENGTH, -0.5),  // -Y edge toward corner 1
    vec3( 0.5,  0.5, -0.5), vec3( 0.5,  0.5, -0.5 + LINE_LENGTH),  // +Z edge toward corner 6
    // Corner 3 (-0.5,0.5,-0.5) back-top-left - yellow
    vec3(-0.5,  0.5, -0.5), vec3(-0.5 + LINE_LENGTH,  0.5, -0.5),  // +X edge toward corner 2
    vec3(-0.5,  0.5, -0.5), vec3(-0.5,  0.5 - LINE_LENGTH, -0.5),  // -Y edge toward corner 0
    vec3(-0.5,  0.5, -0.5), vec3(-0.5,  0.5, -0.5 + LINE_LENGTH),  // +Z edge toward corner 7
    // Corner 4 (-0.5,-0.5,0.5) front-bottom-left - magenta
    vec3(-0.5, -0.5,  0.5), vec3(-0.5 + LINE_LENGTH, -0.5,  0.5),  // +X edge toward corner 5
    vec3(-0.5, -0.5,  0.5), vec3(-0.5, -0.5 + LINE_LENGTH,  0.5),  // +Y edge toward corner 7
    vec3(-0.5, -0.5,  0.5), vec3(-0.5, -0.5,  0.5 - LINE_LENGTH),  // -Z edge toward corner 0
    // Corner 5 (0.5,-0.5,0.5) front-bottom-right - cyan
    vec3( 0.5, -0.5,  0.5), vec3( 0.5 - LINE_LENGTH, -0.5,  0.5),  // -X edge toward corner 4
    vec3( 0.5, -0.5,  0.5), vec3( 0.5, -0.5 + LINE_LENGTH,  0.5),  // +Y edge toward corner 6
    vec3( 0.5, -0.5,  0.5), vec3( 0.5, -0.5,  0.5 - LINE_LENGTH),  // -Z edge toward corner 1
    // Corner 6 (0.5,0.5,0.5) front-top-right - orange
    vec3( 0.5,  0.5,  0.5), vec3( 0.5 - LINE_LENGTH,  0.5,  0.5),  // -X edge toward corner 7
    vec3( 0.5,  0.5,  0.5), vec3( 0.5,  0.5 - LINE_LENGTH,  0.5),  // -Y edge toward corner 5
    vec3( 0.5,  0.5,  0.5), vec3( 0.5,  0.5,  0.5 - LINE_LENGTH),  // -Z edge toward corner 2
    // Corner 7 (-0.5,0.5,0.5) front-top-left - gray
    vec3(-0.5,  0.5,  0.5), vec3(-0.5 + LINE_LENGTH,  0.5,  0.5),  // +X edge toward corner 6
    vec3(-0.5,  0.5,  0.5), vec3(-0.5,  0.5 - LINE_LENGTH,  0.5),  // -Y edge toward corner 4
    vec3(-0.5,  0.5,  0.5), vec3(-0.5,  0.5,  0.5 - LINE_LENGTH)   // -Z edge toward corner 3
);

void main() {
    // vec3 scaleFactors = vec3(2.0, 2.0, 2.0);
    // mat4 scaleMatrix = getScaleMatrix(scaleFactors);

    vec4 vertex = boxTrnasformToObject.matrix* vec4(lineVerts[gl_VertexIndex], 1.0);
    gl_Position = pc.projection * pc.modelview * vertex;

    fragColor = colors[gl_VertexIndex / 6];
}
