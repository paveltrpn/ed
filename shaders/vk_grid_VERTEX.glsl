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

vec4 vertecies[6] = vec4[](
        vec4( 1.0, -1.0, 0.0, 1.0 ), vec4(  1.0,  1.0, 0.0, 1.0 ), vec4(-1.0,  1.0, 0.0, 1.0 ),
        vec4( -1.0, 1.0, 0.0, 1.0 ), vec4( -1.0, -1.0, 0.0, 1.0 ), vec4( 1.0, -1.0, 0.0, 1.0 ) );

void main() {
    // mat4 planeScale = mat4(0.0);
    // planeScale[0][0] = scale;
    // planeScale[1][1] = 1.0;
    // planeScale[2][2] = scale;
    // planeScale[3][3] = 1.0;

    // mat4 planeYOffset = mat4(0.0);
    // planeYOffset[3][0] = 0.0f;
    // planeYOffset[3][1] = yOffset;
    // planeYOffset[3][2] = 0.0f;
    // planeYOffset[3][3] = 1.0f;

    // planeYOffset[0][0] = 1.0;
    // planeYOffset[1][1] = 1.0;
    // planeYOffset[2][2] = 1.0;
    // planeYOffset[3][3] = 1.0;

    // vec4 worldPos = planeScale * planeYOffset * vertecies[gl_VertexID];

    // vWorldPos = -worldPos.xyz;
    // gl_Position = view_matrix * model_matrix * worldPos;

    mat4 planeScale = mat4(0.0f);
    planeScale[0][0] = planeParams.scale;
    planeScale[1][1] = planeParams.scale;
    planeScale[2][2] = 1.0f;
    planeScale[3][3] = 1.0f;

    mat4 planeZOffset= mat4(0.0f);
    planeZOffset[3][0] = 0.0f;
    planeZOffset[3][1] = 0.0f;
    planeZOffset[3][2] = planeParams.zOffset;
    planeZOffset[3][3] = 1.0f;

    planeZOffset[0][0] = 1.0f;
    planeZOffset[1][1] = 1.0f;
    planeZOffset[2][2] = 1.0f;
    planeZOffset[3][3] = 1.0f;

    vec4 worldPos = planeScale * planeZOffset * vertecies[gl_VertexIndex];
    vWorldPos = worldPos.xyz;
    gl_Position = pc.projection * pc.modelview * worldPos;
}

