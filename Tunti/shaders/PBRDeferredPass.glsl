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

const float PI = 3.14159265359f;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 v_TexCoord;

struct Light
{
    vec4 AttenuationFactors; // x: Intensity, y: Constant, z: Linear, w: Quadratic
    vec4 ColorAndType; // rgb: Color, a: Type
    vec3 Position;
    vec3 Direction;
};

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 ViewProjection;
	vec3 CameraPosition;
};

layout(std140, binding = 4) uniform LightsUniform
{
    int NumLights;
    Light Lights[MAX_LIGHTS];
};

uniform sampler2D u_PositionAttachment;
uniform sampler2D u_NormalAttachment;
uniform sampler2D u_AlbedoSpecularAttachment;

/*
    Function declarations
*/
float Fd90(float NoL, float roughness);
float KDisneyTerm(float NoL, float NoV, float roughness);
vec3 FresnelSchlick(float NdotV, vec3 F0);
vec3 FresnelSchlickRoughness(float NdotV, vec3 F0, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometryAttenuationGGXSmith(float NdotL, float NdotV, float roughness);

void main()
{
    vec3 worldPos = texture(u_PositionAttachment, v_TexCoord).rgb;

    // Material Properties
    vec3 albedo = pow(texture(u_AlbedoSpecularAttachment, v_TexCoord).rgb, vec3(2.2));
    float metalness = texture(u_AlbedoSpecularAttachment, v_TexCoord).a;
    float roughness = texture(u_NormalAttachment, v_TexCoord).a;
    float ambientOcclusion = texture(u_PositionAttachment, v_TexCoord).a;

    // Lighting Data
    vec3 N = texture(u_NormalAttachment, v_TexCoord).rgb;
    vec3 V = normalize(CameraPosition - worldPos);
    vec3 R = reflect(-V, N);

    // Reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), albedo, metalness);

    // Reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < NumLights; i++)
    {
        // Per-light radiance
        Light light = Lights[i];
        vec3 L = -1.0 * normalize(light.Direction);
        vec3 H = normalize(L + V);

        float attenuation = 1.0;

        vec3 radiance = light.ColorAndType.rgb * light.AttenuationFactors.x * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometryAttenuationGGXSmith(dot(N, L), dot(N, V), roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metalness;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    // Temporary till IBL
    vec3 ambient = vec3(0.03) * albedo * ambientOcclusion;
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, 1.0);
}

/*
    PBR Functions
*/

float Fd90(float NoL, float roughness)
{
    return (2.0f * NoL * roughness) + 0.4f;
}

float KDisneyTerm(float NoL, float NoV, float roughness)
{
    return (1.0f + Fd90(NoL, roughness) * pow(1.0f - NoL, 5.0f)) * (1.0f + Fd90(NoV, roughness) * pow(1.0f - NoV, 5.0f));
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 FresnelSchlickRoughness(float NdotV, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - NdotV, 5.0f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;

    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    return (alpha2) / (PI * (NdotH2 * (alpha2 - 1.0f) + 1.0f) * (NdotH2 * (alpha2 - 1.0f) + 1.0f));
}

float GeometryAttenuationGGXSmith(float NdotL, float NdotV, float roughness)
{
    float NdotL2 = NdotL * NdotL;
    float NdotV2 = NdotV * NdotV;
    float kRough2 = roughness * roughness + 0.0001f;

    float ggxL = (2.0f * NdotL) / (NdotL + sqrt(NdotL2 + kRough2 * (1.0f - NdotL2)));
    float ggxV = (2.0f * NdotV) / (NdotV + sqrt(NdotV2 + kRough2 * (1.0f - NdotV2)));

    return ggxL * ggxV;
}