#type vertex
#version 450 core

layout(location = 0) out VertexInOut
{
	vec2 TexCoord;
} VertexOut;

void main() 
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);

    gl_Position = vec4(-1.0f + x*2.0f, -1.0f+y*2.0f, 0.0f, 1.0f);
    VertexOut.TexCoord = vec2(x, y);
}

#type fragment
#version 450 core
#extension GL_ARB_bindless_texture : require
#define MAX_LIGHTS 100

const float PI = 3.14159265359f;

layout(location = 0) out vec4 outColor;

layout(location = 0) in VertexInOut
{
	vec2 TexCoord;
} VertexIn;

struct Light
{
    vec4 AttenuationFactors; // x: Intensity, y: Constant, z: Linear, w: Quadratic
    vec4 ColorAndType; // rgb: Color, a: Type
    vec3 Position;
    vec3 Direction;
};

struct DirectionalLight
{
    vec4 ColorAndIntensity;
    vec3 Direction;
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
float Saturate(float f);
vec3 LinearizeColor(vec3 color);
float Fd90(float NoL, float roughness);
float KDisneyTerm(float NoL, float NoV, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometryAttenuationGGXSmith(float NdotL, float NdotV, float roughness);
vec3 FresnelSchlick(float NdotV, vec3 F0);

void main()
{
    vec3 viewPos = texture(u_PositionAttachment, VertexIn.TexCoord).rgb;

    // Material Properties
    vec3 albedo = LinearizeColor(texture(u_AlbedoSpecularAttachment, VertexIn.TexCoord).rgb);
    vec3 normal = texture(u_NormalAttachment, VertexIn.TexCoord).rgb;
    float metalness = texture(u_AlbedoSpecularAttachment, VertexIn.TexCoord).a;
    float roughness = texture(u_NormalAttachment, VertexIn.TexCoord).a;
    float ambientOcclusion = texture(u_PositionAttachment, VertexIn.TexCoord).a;

    vec3 V = normalize(-viewPos);
    vec3 N = normalize(normal);
    vec3 R = reflect(-V, N);

    float NdotV = max(dot(N, V), 0.0001f);

    vec3 F0 = mix(vec3(0.04f), albedo, metalness);
    vec3 F = FresnelSchlick(NdotV, F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0f) - kS;
    kD *= 1.0f - metalness;

    vec3 color = vec3(0.0f);

    // Calculate Directional Lights
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    for(int i = 0; i < NumLights; i++)
    {
        Light light = Lights[i];
        vec3 L = normalize(-light.Direction);
        vec3 H = normalize(L + V);

        vec3 lightColor = LinearizeColor(light.ColorAndType.rgb) * light.AttenuationFactors.x;

        float NdotL = Saturate(dot(N, L));

        diffuse = albedo / PI;

        float kDisney = KDisneyTerm(NdotL, NdotV, roughness);

        float D = DistributionGGX(N, H, roughness);

        float G = GeometryAttenuationGGXSmith(NdotL, NdotV, roughness);

        specular = (F * D * G) / (4.0f * NdotL * NdotV + 0.0001f);

        color += (diffuse * kDisney * kD + specular) * lightColor * NdotL;
    }

    // Temporary till IBL
    vec3 ambient = vec3(0.03f) * albedo * ambientOcclusion;
    color += ambient;

    // HDR tonemapping
    color = color / (color + vec3(1.0f));
    // gamma correct
    color = pow(color, vec3(1.0f / 2.2f));

    outColor = vec4(color, 1.0f);
}

/*
    Utility Functions
*/

vec3 LinearizeColor(vec3 color)
{
    return pow(color.rgb, vec3(2.2f));
}

float Saturate(float f)
{
    return clamp(f, 0.0f, 1.0f);
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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;

    float NdotH = Saturate(dot(N, H));
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

vec3 FresnelSchlick(float NdotV, vec3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - NdotV, 5.0f);
}