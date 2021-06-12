#type vertex
#version 450 core

layout(location = 0) out VertexInOut
{
	vec2 TexCoord;
} VertexOut;

void main()
{
    float x = -1.0f + float((gl_VertexID & 1) << 2);
	float y = -1.0f + float((gl_VertexID & 2) << 1);
    VertexOut.TexCoord = vec2((x + 1.0f) * 0.5f, (y + 1.0f) * 0.5f);
	gl_Position = vec4(x, y, 0.0f, 1.0f);
}

#type fragment
#version 450 core
#define MAX_POINT_LIGHTS 100

const float Epsilon = 1e-5f;
const float PI = 3.14159265359f;
const vec3 Fdielectric = vec3(0.04f);

layout(location = 0) out vec4 outColor;

layout(location = 0) in VertexInOut
{
	vec2 TexCoord;
} VertexIn;

struct DirectionalLight
{
    mat4 ViewProjection;
    vec4 DirectionAndIntensity;
    vec4 Color;
};

struct PointLight
{
    vec4 Position;
    vec4 Color;
    vec4 ConstantLinearQuadratic;
};

layout(std140, binding = 0) uniform CameraUniform
{
    mat4 View;
	mat4 Projection;
	mat4 InvView;
	mat4 InvProjection;
	vec3 Position;
} Camera;

layout(std140, binding = 4) uniform LightsUniform
{
    DirectionalLight directionalLight;
    PointLight PointLights[MAX_POINT_LIGHTS];
    int NumPointLights;
};

uniform float u_EnvironmentMapIntensity;
uniform sampler2D u_DepthAttachment;
uniform sampler2D u_NormalAttachment;
uniform sampler2D u_AlbedoAttachment;
uniform sampler2D u_LightSpacePositionAttachment;
uniform samplerCube u_IrradianceCubemap;
uniform samplerCube u_SpecularCubemap;
uniform sampler2D u_SpecularBRDF_LUT;
uniform sampler2DShadow u_DirectionalLightShadowMap;
uniform samplerCubeShadow u_PointLightShadowMaps[MAX_POINT_LIGHTS];

/*
    Function declarations
*/
float DistributionGGX(float cosLh, float roughness);
float GeometrySchlickGGX(float cosTheta, float k);
float GeometrySmith(float cosLi, float cosLo, float roughness);
vec3 FresnelSchlick(vec3 F0, float cosTheta);
vec3 FresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness);
vec3 FragPosFromDepth(float depth);
float CalculateDirectionalShadow(vec4 fragPosLightSpace);
vec3 CalculateDirectionalLighting(vec3 V, float NdotV, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metalness);
vec3 CalculateAmbientLighting(vec3 R, float NdotV, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metalness);

void main()
{
    vec3 fragPos = FragPosFromDepth(texture(u_DepthAttachment, VertexIn.TexCoord).r);
    vec4 fragPosLightSpace = texture(u_LightSpacePositionAttachment, VertexIn.TexCoord);

    vec4 gNormal = texture(u_NormalAttachment, VertexIn.TexCoord);
    vec4 gAlbedo = texture(u_AlbedoAttachment, VertexIn.TexCoord);

    // Material Properties
    vec3 normal = gNormal.rgb;
    vec3 albedo = gAlbedo.rgb;
    float roughness = gNormal.a;
    float metalness = gAlbedo.a;

    normal = normalize(normal);

    // Outgoing light direction (vector from world-space fragment position to the "eye").
    vec3 V = normalize(Camera.Position - fragPos);

    // Specular reflection vector
    vec3 R = reflect(-V, normal);

    // Fresnel reflectance at normal incidence (for metals use albedo color)
    vec3 F0 = mix(Fdielectric, albedo, metalness);

    // Angle between surface normal and outgoing light direction
    float NdotV = max(dot(normal, V), 0.0f);

    // Calculate Directional Lighting
    vec3 directionalLighting = CalculateDirectionalLighting(V, NdotV, normal, F0, albedo, roughness, metalness);

    // Diffuse lighting
    float shadow = CalculateDirectionalShadow(fragPosLightSpace);
    vec3 diffuseLighting = directionalLighting * (1.0f - shadow);

    // Ambient lighting (IBL).
	vec3 ambientLighting = CalculateAmbientLighting(R, NdotV, normal, F0, albedo, roughness, metalness);

    // Combine to get the final color
    vec3 color = ambientLighting + diffuseLighting;
    
    // HDR tonemapping
    color = color / (color + vec3(1.0f));
    // gamma correction
    color = pow(color, vec3(1.0f / 2.2f));

    outColor = vec4(color, 1.0f);
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float DistributionGGX(float NdotH, float roughness)
{
    float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (NdotH * NdotH) * (alphaSq - 1.0f) + 1.0f;
	return alphaSq / max((PI * denom * denom), Epsilon);
}

// Single term for separable Schlick-GGX below.
float GeometrySchlickGGX(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0f - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    return GeometrySchlickGGX(NdotV, k) * GeometrySchlickGGX(NdotL, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 FresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (vec3(1.0f) - F0) * pow(max(1.0f - cosTheta, 0.0f), 5.0f);
}

// Shlick's approximation of the Fresnel factor for roughness.
vec3 FresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(max(1.0f - cosTheta, 0.0f), 5.0f);
}

// Retrieve Fragment Position from Depth Buffer
vec3 FragPosFromDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;

	vec4 clipspaceposition = vec4(VertexIn.TexCoord * 2.0f - 1.0f, z, 1.0f);
	vec4 viewspaceposition = Camera.InvProjection * clipspaceposition;

	viewspaceposition /= viewspaceposition.w;

	vec4 worldspaceposition = Camera.InvView * viewspaceposition;
	return worldspaceposition.xyz;
}

// Shadow Mapping
float CalculateDirectionalShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5f + 0.5f;

    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(u_DirectionalLightShadowMap, 0);
    for (int y = -1 ; y <= 1 ; y++) 
    {
        for (int x = -1 ; x <= 1 ; x++) 
        {
            vec2 uv = projCoords.xy + vec2(x, y) * texelSize;
            shadow += texture(u_DirectionalLightShadowMap, vec3(uv, projCoords.z + Epsilon));
        }
    }
    return (0.5f + (shadow / 18.0f));
}

// Directional Lighting
vec3 CalculateDirectionalLighting(vec3 V, float NdotV, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metalness)
{
    vec3 L = -directionalLight.DirectionAndIntensity.xyz; // Light direction vector
    vec3 H = normalize(V + L); // Halfway vector

    vec3 Lradiance = directionalLight.Color.rgb * directionalLight.DirectionAndIntensity.w;

    // Calculate angles between surface normal and various light vectors
    float NdotL = max(dot(normal, L), 0.0f);
    float NdotH = max(dot(normal, H), 0.0f);

    // Calculate Fresnel term for direct lighting. 
    vec3 F  = FresnelSchlick(F0, max(dot(H, V), 0.0f));
    // Calculate normal distribution for specular BRDF.
    float D = DistributionGGX(NdotH, roughness);
    // Calculate geometric attenuation for specular BRDF.
    float G = GeometrySmith(NdotV, NdotL, roughness);

    // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
    // Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
    // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
    vec3 Kd = mix(vec3(1.0f) - F, vec3(0.0f), metalness);

    // Lambert diffuse BRDF.
    // We don't scale by 1/PI for lighting & material units to be more convenient.
    // See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    vec3 diffuseBRDF = Kd * albedo;

    // Cook-Torrance specular microfacet BRDF.
    vec3 specularBRDF = (D * G * F) / (4.0f * NdotV * NdotL + Epsilon);

    // Total contribution for this light.
    return (diffuseBRDF + specularBRDF) * Lradiance * NdotL * directionalLight.Color.a;
}

// Image-Based Lighting (Ambient Lighting)
vec3 CalculateAmbientLighting(vec3 R, float NdotV, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metalness)
{
    // Calculate Fresnel term for ambient lighting.
    // Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
    // use cosLo instead of angle with light's half-vector (cosLh above).
    // See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
    vec3 F = FresnelSchlick(F0, NdotV);

    // Get diffuse contribution factor (as with direct lighting).
    vec3 Kd = (1.0f - F) * (1.0f - metalness);

    // Sample diffuse irradiance at normal direction.
    vec3 irradiance = texture(u_IrradianceCubemap, normal).rgb;

     // Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
    vec3 diffuseIBL = Kd * irradiance * albedo;

    // Sample pre-filtered specular reflection environment at correct mipmap level.
    int specularTextureLevels = textureQueryLevels(u_SpecularCubemap);
    vec3 specularIrradiance = textureLod(u_SpecularCubemap, R, roughness * specularTextureLevels).rgb;

    // Split-sum approximation factors for Cook-Torrance specular BRDF.
    vec2 specularBRDF = texture(u_SpecularBRDF_LUT, vec2(NdotV, roughness)).rg;

    // Total specular IBL contribution.
    vec3 specularIBL = specularIrradiance * (F * specularBRDF.x + specularBRDF.y) ;

    // Total ambient lighting contribution.
    return (diffuseIBL + specularIBL) * u_EnvironmentMapIntensity;
}