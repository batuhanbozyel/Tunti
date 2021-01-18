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

#define MAX_LIGHTS_PER_TYPE 100

layout(location = 0) out vec4 color;

layout(location = 0) in vec2 v_TexCoord;

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_Position;
};

struct PointLight
{
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;

    float Radius;
};

layout(std140, binding = 3) uniform LightsUniform
{
    PointLight u_PointLights[MAX_LIGHTS_PER_TYPE];
    int u_NumLights;
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

    vec3 lighting = Albedo * 0.1;
    vec3 viewDir = normalize(u_Position - FragPos);
    for(int i = 0; i < u_NumLights; i++)
    {
        float distance = length(u_PointLights[i].Position - FragPos);
        if(distance < u_PointLights[i].Radius)
        {
            // Albedo
            vec3 lightDir = normalize(u_PointLights[i].Position - FragPos);
            vec3 albedo = max(dot(Normal, lightDir), 0.0) * Albedo * u_PointLights[i].Color;

            // Specular
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = u_PointLights[i].Color * spec * Specular;

            // Attenuation
            float attenuation = 1.0 / (1.0 + u_PointLights[i].Linear * distance + u_PointLights[i].Quadratic * distance * distance);
            albedo *= attenuation;
            specular *= attenuation;
            lighting += albedo + specular;
        }
    }
    color = vec4(lighting, 1.0);
}