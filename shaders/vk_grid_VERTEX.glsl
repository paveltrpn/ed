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
    mat4 planeScale = mat4(1.0);
    planeScale[0][0] = planeParams.scale;
    planeScale[1][1] = planeParams.scale;

    mat4 planeZOffset= mat4(1.0);
    planeZOffset[3][2] = planeParams.zOffset;

    vec4 worldPos = planeScale * planeZOffset * vertecies[gl_VertexIndex];
    vWorldPos = worldPos.xyz;
    gl_Position = pc.projection * pc.modelview * worldPos;
}

