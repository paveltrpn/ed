#version 450

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
} pc;

layout(set = 0, binding = 0) uniform PlaneParamsBuffer {
    float scale;
    float zOffset;
    float _p1;
    float _p2;
} planeParams;

layout(location = 0) out vec3 vWorldPos;

vec4 vertices[6] = vec4[](
        vec4( 1.0, -1.0, 0.0, 1.0 ), vec4(  1.0,  1.0, 0.0, 1.0 ), vec4(-1.0,  1.0, 0.0, 1.0 ),
        vec4( -1.0, 1.0, 0.0, 1.0 ), vec4( -1.0, -1.0, 0.0, 1.0 ), vec4( 1.0, -1.0, 0.0, 1.0 ) );

void main() {
    mat4 planeScale = mat4(0.0f);
    planeScale[0][0] = planeParams.scale;
    planeScale[1][1] = planeParams.scale;
    planeScale[2][2] = 1.0f;
    planeScale[3][3] = 1.0f;

    mat4 planeZOffset = mat4(0.0f);
    planeZOffset[3][0] = 0.0f;
    planeZOffset[3][1] = 0.0f;
    planeZOffset[3][2] = planeParams.zOffset;
    planeZOffset[3][3] = 1.0f;

    planeZOffset[0][0] = 1.0f;
    planeZOffset[1][1] = 1.0f;
    planeZOffset[2][2] = 1.0f;
    planeZOffset[3][3] = 1.0f;

    vec4 worldPos = planeScale * planeZOffset * vertices[gl_VertexIndex];
    vWorldPos = worldPos.xyz;
    gl_Position = pc.projection * pc.modelview * worldPos;
}

