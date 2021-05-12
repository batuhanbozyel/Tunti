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

const float Epsilon = 0.00001f;
const float PI = 3.14159265359f;
const vec3 Fdielectric = vec3(0.04f);

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

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 View;
	mat4 Projection;
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
uniform samplerCube u_IrradianceCubemap;
uniform samplerCube u_SpecularCubemap;
uniform sampler2D u_SpecularBRDF_LUT;

/*
    Function declarations
*/
float DistributionGGX(float cosLh, float roughness);
float GeometrySchlickGGX(float cosTheta, float k);
float GeometrySmith(float cosLi, float cosLo, float roughness);
vec3 FresnelSchlick(vec3 F0, float cosTheta);

void main()
{
    vec3 worldPos = texture(u_PositionAttachment, VertexIn.TexCoord).rgb;

    // Material Properties
    vec3 albedo = texture(u_AlbedoSpecularAttachment, VertexIn.TexCoord).rgb;
    vec3 normal = texture(u_NormalAttachment, VertexIn.TexCoord).rgb;
    float metalness = texture(u_AlbedoSpecularAttachment, VertexIn.TexCoord).a;
    float roughness = texture(u_NormalAttachment, VertexIn.TexCoord).a;
    float ambientOcclusion = texture(u_PositionAttachment, VertexIn.TexCoord).a;

    // Outgoing light direction (vector from world-space fragment position to the "eye").
    vec3 Lo = normalize(CameraPosition - worldPos);

    // Angle between surface normal and outgoing light direction
    float cosLo = max(0.0f, dot(normal, Lo));

    // Specular reflection vector
    vec3 Lr = 2.0f * cosLo * normal - Lo;

    // Fresnel reflectance at normal incidence (for metals use albedo color)
    vec3 F0 = mix(Fdielectric, albedo, metalness);

    vec3 directionalLighting = vec3(0.0f);
    for(int i = 0; i < NumLights; i++)
    {
        Light light = Lights[i];
        vec3 Li = -light.Direction;
        float intensity = light.AttenuationFactors.x;
        vec3 Lradiance = light.ColorAndType.rgb * intensity;

        // Half-vector between Li and Lo
        vec3 Lh = normalize(Li + Lo);

        // Calculate angles between surface normal and various light vectors
        float cosLi = max(0.0f, dot(normal, Li));
        float cosLh = max(0.0f, dot(normal, Lh));

        // Calculate Fresnel term for direct lighting. 
		vec3 F  = FresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		// Calculate normal distribution for specular BRDF.
		float D = DistributionGGX(cosLh, roughness);
		// Calculate geometric attenuation for specular BRDF.
		float G = GeometrySmith(cosLi, cosLo, roughness);
        
        // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
		vec3 kd = mix(vec3(1.0f) - F, vec3(0.0f), metalness);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
		vec3 diffuseBRDF = kd * albedo;

		// Cook-Torrance specular microfacet BRDF.
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// Total contribution for this light.
		directionalLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }

    // Ambient lighting (IBL).
	vec3 ambientLighting;
	{
		// Sample diffuse irradiance at normal direction.
		vec3 irradiance = texture(u_IrradianceCubemap, normal).rgb;

		// Calculate Fresnel term for ambient lighting.
		// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
		// use cosLo instead of angle with light's half-vector (cosLh above).
		// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
		vec3 F = FresnelSchlick(F0, cosLo);

		// Get diffuse contribution factor (as with direct lighting).
		vec3 kd = mix(vec3(1.0f) - F, vec3(0.0f), metalness);

		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		vec3 diffuseIBL = kd * albedo * irradiance;

		// Sample pre-filtered specular reflection environment at correct mipmap level.
		int specularTextureLevels = textureQueryLevels(u_SpecularCubemap);
		vec3 specularIrradiance = textureLod(u_SpecularCubemap, Lr, roughness * specularTextureLevels).rgb;

		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		vec2 specularBRDF = texture(u_SpecularBRDF_LUT, vec2(cosLo, roughness)).rg;

		// Total specular IBL contribution.
		vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// Total ambient lighting contribution.
		ambientLighting = (diffuseIBL + specularIBL) * ambientOcclusion;
	}

    vec3 color = directionalLighting + ambientLighting;
    
    // HDR tonemapping
    color = color / (color + vec3(1.0f));
    // gamma correction
    color = pow(color, vec3(1.0f / 2.2f));

    outColor = vec4(color, 1.0f);
}

/*
    PBR Functions
*/

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float DistributionGGX(float cosLh, float roughness)
{
    float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0f) + 1.0f;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float GeometrySchlickGGX(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0f - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float GeometrySmith(float cosLi, float cosLo, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    return GeometrySchlickGGX(cosLi, k) * GeometrySchlickGGX(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 FresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (vec3(1.0f) - F0) * pow(1.0f - cosTheta, 5.0f);
}