#version 450

layout( location = 0 ) out vec3 fragColor;
layout( location = 1 ) out vec3 vLighting;

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
} pc;

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

vec3 color = vec3( 0.4, 0.9, 0.7 );

vec3 ambientLight   = vec3( 0.3, 0.3, 0.3 );
vec3 lightcolor     = vec3( 1.0, 1.0, 1.0 );
vec3 lightpos       = normalize( vec3( 10.0, 0.0, 10.0 ) );

void main() {
    vec4 nrmTransformed     = inverse(transpose( pc.modelview )) * vec4( normals[gl_VertexIndex], 0.0 );
    float directional       = max( dot( nrmTransformed.xyz, lightpos ), 0.0 );
    vLighting               = ambientLight + ( lightcolor * directional );
    gl_Position             = pc.projection * pc.modelview * vec4( positions[gl_VertexIndex], 1.0 );
    fragColor               = color;
}

