#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
    mat4 transform;
} pc;

layout(location = 0) out vec3 fragColor;

out gl_PerVertex {
    vec4 gl_Position;
};

vec3 lineVerts[24] = vec3[](
    // Back face (z = -0.5)
    vec3(-0.5, -0.5, -0.5), vec3( 0.5, -0.5, -0.5),  // edge 0-1
    vec3( 0.5, -0.5, -0.5), vec3( 0.5,  0.5, -0.5),  // edge 1-2
    vec3( 0.5,  0.5, -0.5), vec3(-0.5,  0.5, -0.5),  // edge 2-3
    vec3(-0.5,  0.5, -0.5), vec3(-0.5, -0.5, -0.5),  // edge 3-0
    // Front face (z = +0.5)
    vec3(-0.5, -0.5,  0.5), vec3( 0.5, -0.5,  0.5),  // edge 4-5
    vec3( 0.5, -0.5,  0.5), vec3( 0.5,  0.5,  0.5),  // edge 5-6
    vec3( 0.5,  0.5,  0.5), vec3(-0.5,  0.5,  0.5),  // edge 6-7
    vec3(-0.5,  0.5,  0.5), vec3(-0.5, -0.5,  0.5),  // edge 7-4
    // Pillars
    vec3(-0.5, -0.5, -0.5), vec3(-0.5, -0.5,  0.5),  // edge 0-4
    vec3( 0.5, -0.5, -0.5), vec3( 0.5, -0.5,  0.5),  // edge 1-5
    vec3( 0.5,  0.5, -0.5), vec3( 0.5,  0.5,  0.5),  // edge 2-6
    vec3(-0.5,  0.5, -0.5), vec3(-0.5,  0.5,  0.5)   // edge 3-7
);

void main() {
    vec4 vertex = pc.transform * vec4(lineVerts[gl_VertexIndex], 1.0);
    gl_Position = pc.projection * pc.modelview * vertex;

    fragColor = vec3(0.2, 0.4, 0.5);
}
