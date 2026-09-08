#version 450

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
} pc;

layout(set = 0, binding = 0) uniform BoxParamsBuffer {
    vec3 origin;
    float size;
    vec3 axis;
    float angl;
    vec4 color;
} boxParams;

layout(set = 0, binding = 1) uniform LightParamsBuffer {
    vec3 origin;
    float _p1;
    vec3 color;
    float _p2;
} lightParams;

layout( location = 0 ) out vec3 fragColor;
layout( location = 1 ) out vec3 vLighting;

vec3 positions[36] = vec3[](
        vec3( -0.5, 0.5, -0.5 ), vec3( 0.5, 0.5, 0.5 ), vec3( 0.5, 0.5, -0.5 ),
        vec3( 0.5, 0.5, 0.5 ), vec3( -0.5, -0.5, 0.5 ), vec3( 0.5, -0.5, 0.5 ),
        vec3( -0.5, 0.5, 0.5 ), vec3( -0.5, -0.5, -0.5 ), vec3( -0.5, -0.5, 0.5 ),
        vec3( 0.5, -0.5, -0.5 ), vec3( -0.5, -0.5, 0.5 ), vec3( -0.5, -0.5, -0.5 ),
        vec3( 0.5, 0.5, -0.5 ), vec3( 0.5, -0.5, 0.5 ), vec3( 0.5, -0.5, -0.5 ),
        vec3( -0.5, 0.5, -0.5 ), vec3( 0.5, -0.5, -0.5 ), vec3( -0.5, -0.5, -0.5 ),
        vec3( -0.5, 0.5, -0.5 ), vec3( -0.5, 0.5, 0.5 ), vec3( 0.5, 0.5, 0.5 ),
        vec3( 0.5, 0.5, 0.5 ), vec3( -0.5, 0.5, 0.5 ), vec3( -0.5, -0.5, 0.5 ),
        vec3( -0.5, 0.5, 0.5 ), vec3( -0.5, 0.5, -0.5 ), vec3( -0.5, -0.5, -0.5 ),
        vec3( 0.5, -0.5, -0.5 ), vec3( 0.5, -0.5, 0.5 ), vec3( -0.5, -0.5, 0.5 ),
        vec3( 0.5, 0.5, -0.5 ), vec3( 0.5, 0.5, 0.5 ), vec3( 0.5, -0.5, 0.5 ),
        vec3( -0.5, 0.5, -0.5 ), vec3( 0.5, 0.5, -0.5 ), vec3( 0.5, -0.5, -0.5 ) );

vec3 normals[36] = vec3[](
        vec3( -0.0, 1.0, -0.0 ), vec3( -0.0, 1.0, -0.0 ), vec3( -0.0, 1.0, -0.0 ),
        vec3( -0.0, -0.0, 1.0 ), vec3( -0.0, -0.0, 1.0 ), vec3( -0.0, -0.0, 1.0 ),
        vec3( -1.0, -0.0, -0.0 ), vec3( -1.0, -0.0, -0.0 ),
        vec3( -1.0, -0.0, -0.0 ), vec3( -0.0, -1.0, -0.0 ),
        vec3( -0.0, -1.0, -0.0 ), vec3( -0.0, -1.0, -0.0 ), vec3( 1.0, -0.0, -0.0 ),
        vec3( 1.0, -0.0, -0.0 ), vec3( 1.0, -0.0, -0.0 ), vec3( -0.0, -0.0, -1.0 ),
        vec3( -0.0, -0.0, -1.0 ), vec3( -0.0, -0.0, -1.0 ), vec3( -0.0, 1.0, -0.0 ),
        vec3( -0.0, 1.0, -0.0 ), vec3( -0.0, 1.0, -0.0 ), vec3( -0.0, -0.0, 1.0 ),
        vec3( -0.0, -0.0, 1.0 ), vec3( -0.0, -0.0, 1.0 ), vec3( -1.0, -0.0, -0.0 ),
        vec3( -1.0, -0.0, -0.0 ), vec3( -1.0, -0.0, -0.0 ),
        vec3( -0.0, -1.0, -0.0 ), vec3( -0.0, -1.0, -0.0 ),
        vec3( -0.0, -1.0, -0.0 ), vec3( 1.0, -0.0, -0.0 ), vec3( 1.0, -0.0, -0.0 ),
        vec3( 1.0, -0.0, -0.0 ), vec3( -0.0, -0.0, -1.0 ), vec3( -0.0, -0.0, -1.0 ),
        vec3( -0.0, -0.0, -1.0 ) );

mat4 rotationMatrix( vec3 axis, float angle ) {
    axis = normalize( axis );
    float s = sin( angle );
    float c = cos( angle );
    float oc = 1.0 - c;
    
    return mat4(
        oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
        oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
        oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
        0.0,                                0.0,                                0.0,                                1.0
    );
}

vec3 ambientLight   = vec3( 0.3, 0.3, 0.3 );
vec3 lightpos       = normalize( lightParams.origin );

void main() {
    mat4 boxOffset= mat4(0.0f);
    boxOffset[3][0] = boxParams.origin.x;
    boxOffset[3][1] = boxParams.origin.y;
    boxOffset[3][2] = boxParams.origin.z;

    boxOffset[0][0] = 1.0f;
    boxOffset[1][1] = 1.0f;
    boxOffset[2][2] = 1.0f;
    boxOffset[3][3] = 1.0f;

    mat4 boxScale= mat4(0.0f);
    boxScale[0][0] = boxParams.size;
    boxScale[1][1] = boxParams.size;
    boxScale[2][2] = boxParams.size;
    boxScale[3][3] = 1.0f;

    mat4 boxRot = rotationMatrix( boxParams.axis, boxParams.angl );

    vec4 worldPos = boxRot * boxScale * boxOffset * vec4( positions[gl_VertexIndex], 1.0 );
    gl_Position             = pc.projection * pc.modelview * worldPos;

    vec3 nrmRotated = ( inverse(transpose(boxRot)) * vec4( normals[gl_VertexIndex], 0.0 ) ).xyz;
    vec4 nrmTransformed     = inverse(transpose( pc.modelview )) * vec4( nrmRotated, 0.0 );

    float directional       = max( dot( nrmTransformed.xyz, lightpos ), 0.0 );
    vLighting               = ambientLight + ( lightParams.color * directional );

    fragColor               = boxParams.color.rgb;
}

