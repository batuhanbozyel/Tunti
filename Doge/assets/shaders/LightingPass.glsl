#type vertex
#version 450 core

layout(location = 0) out vec2 v_TexCoord;

void main() 
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);

    gl_Position = vec4(-1.0f + x*2.0f, -1.0f+y*2.0f, 0.0f, 1.0f);
    v_TexCoord = vec2(x, y);
}

#type fragment
#version 450 core
#extension GL_ARB_bindless_texture : require
#define MAX_LIGHTS 100

layout(location = 0) out vec4 color;

layout(location = 0) in vec2 v_TexCoord;

struct Light
{
    vec4 LightDir;      // 0

    vec3 Position;      // 16
    vec3 Color;         // 32
    /*
        x: Intensity
        y: Constant
        z: Linear
        w: Quadratic
    */
    vec4 AttenuationFactors; // 48
    uint Type;          // 64
};

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 View;
    mat4 Projection;
	vec3 CameraPosition;
};

layout(std140, binding = 4) uniform LightsUniform
{
    Light Lights[MAX_LIGHTS];
    int NumLights;
};

uniform sampler2D u_PositionAttachment;
uniform sampler2D u_NormalAttachment;
uniform sampler2D u_AlbedoSpecularAttachment;

void main()
{
    vec3 FragPos = texture(u_PositionAttachment, v_TexCoord).rgb;
    vec3 Normal = texture(u_NormalAttachment, v_TexCoord).rgb;
    vec3 Albedo = texture(u_AlbedoSpecularAttachment, v_TexCoord).rgb;
    float Specular = texture(u_AlbedoSpecularAttachment, v_TexCoord).a;

    vec3 dummy = normalize(FragPos + Normal + Albedo + Specular);

    color = vec4(dummy, 1.0);
}