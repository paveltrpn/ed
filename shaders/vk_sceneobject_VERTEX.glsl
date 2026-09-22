#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 vLighting;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    vec3 color = vec3( 0.4, 0.9, 0.7 );
    vec3 ambientLight   = vec3( 0.3, 0.3, 0.3 );
    vec3 lightcolor     = vec3( 1.0, 1.0, 1.0 );
    vec3 lightpos       = normalize( vec3( 1.0, 0.0, 1.0 ) );

    vec4 nrmTransformed     = inverse(transpose( pc.modelview )) * vec4( inNormal, 0.0 );
    float directional       = max( dot( nrmTransformed.xyz, lightpos ), 0.0 );
    vLighting               = ambientLight + ( lightcolor * directional );

    gl_Position             = (pc.projection * pc.modelview) * vec4(inPosition, 1.0);
    fragColor               = inColor;
    fragTexCoord            = inTexCoord;
}
