#type vertex
#version 450 core

layout(location = 0) out VertexInOut
{
	vec3 WorldPosition;
	vec3 Normal;
	vec2 TexCoord;
	flat uint MaterialIndex;
} VertexOut;

layout(std140, binding = 0) uniform CameraUniform
{
    mat4 View;
	mat4 Projection;
	mat4 InvView;
	mat4 InvProjection;
	vec3 Position;
} Camera;

layout(std430, binding = 1) readonly buffer MaterialIndexBuffer
{
	uint indices[];
} materialIndexBuffer;

layout(std430, binding = 2) readonly buffer VertexBuffer
{
	float data[];
} vertexBuffer;

uniform uint u_VertexCount;
uniform mat4 u_Model;

void main()
{
	uint posIndex = gl_VertexID * 3;
	VertexOut.WorldPosition = vec3(u_Model * vec4(
		vertexBuffer.data[posIndex],
		vertexBuffer.data[posIndex + 1],
		vertexBuffer.data[posIndex + 2], 1.0f));

	mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
	uint normalIndex = u_VertexCount * 3 + gl_VertexID * 3;
	VertexOut.Normal = normalMatrix * vec3(
		vertexBuffer.data[normalIndex],
		vertexBuffer.data[normalIndex + 1],
		vertexBuffer.data[normalIndex + 2]);

	uint texCoordIndex = u_VertexCount * 6 + gl_VertexID * 2;
	VertexOut.TexCoord = vec2(
		vertexBuffer.data[texCoordIndex],
		vertexBuffer.data[texCoordIndex + 1]);

	VertexOut.MaterialIndex = materialIndexBuffer.indices[gl_VertexID];

	gl_Position = Camera.Projection * Camera.View * vec4(VertexOut.WorldPosition, 1.0f);
}

#type fragment
#version 450 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec3 g_Normal;
layout(location = 1) out vec3 g_Albedo;
layout(location = 2) out vec3 g_RoughnessMetalnessAO;

layout(location = 0) in VertexInOut
{
	vec3 WorldPosition;
	vec3 Normal;
	vec2 TexCoord;
	flat uint MaterialIndex;
} VertexIn;

struct Material
{
	vec4 Albedo;
	uvec2 AlbedoMap;
	uvec2 NormalMap;
	uvec2 MetalnessMap;
	uvec2 RoughnessMap;
	uvec2 AmbientOcclusionMap;
	float Metalness;
	float Roughness;
};

layout(std430, binding = 3) readonly buffer MaterialBuffer
{
	Material materials[];
} materialBuffer;

vec3 computeNormal(vec3 normalMap);

void main()
{
	Material material = materialBuffer.materials[VertexIn.MaterialIndex];

	g_Normal = computeNormal(texture(sampler2D(material.NormalMap), VertexIn.TexCoord).rgb);

	g_Albedo = texture(sampler2D(material.AlbedoMap), VertexIn.TexCoord).rgb * material.Albedo.rgb;

	g_RoughnessMetalnessAO = vec3(
		texture(sampler2D(material.RoughnessMap), VertexIn.TexCoord).r * material.Roughness,
		texture(sampler2D(material.MetalnessMap), VertexIn.TexCoord).r * material.Metalness,
		texture(sampler2D(material.AmbientOcclusionMap), VertexIn.TexCoord).r);
}

vec3 computeNormal(vec3 normalMap)
{
	vec3 texNormal = normalMap * 2.0f - 1.0f;

	vec3 dPosX  = dFdx(VertexIn.WorldPosition);
    vec3 dPosY  = dFdy(VertexIn.WorldPosition);
    vec2 dTexX = dFdx(VertexIn.TexCoord);
    vec2 dTexY = dFdy(VertexIn.TexCoord);

    vec3 normal = normalize(VertexIn.Normal);
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, binormal, normal);

    return normalize(TBN * texNormal);
}