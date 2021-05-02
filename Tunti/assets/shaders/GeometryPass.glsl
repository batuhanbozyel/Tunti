#type vertex
#version 450 core

layout(location = 0) out VertexInOut
{
	vec3 WorldPosition;
	vec3 Normal;
	vec2 TexCoord;
	flat uint TexIndex;
} VertexOut;

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 View;
	mat4 Projection;
	vec3 CameraPosition;
};

layout(std430, binding = 1) readonly buffer TextureMapIndexBuffer
{
	uint indices[];
} textureMapIndexArray;

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

	VertexOut.TexIndex = textureMapIndexArray.indices[gl_VertexID];

	gl_Position = Projection * View * vec4(VertexOut.WorldPosition, 1.0f);
}

#type fragment
#version 450 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec4 g_Position;
layout(location = 1) out vec4 g_Normal;
layout(location = 2) out vec4 g_AlbedoSpec;

layout(location = 0) in VertexInOut
{
	vec3 WorldPosition;
	vec3 Normal;
	vec2 TexCoord;
	flat uint TexIndex;
} VertexIn;

struct TextureMap
{
	uvec2 Albedo;
	uvec2 Normal;
	uvec2 Metalness;
	uvec2 Roughness;
	uvec2 AmbientOcclusion;
};

layout(std430, binding = 3) readonly buffer TextureMaps
{
	TextureMap textures[];
} textureArray;

vec3 computeNormal();

void main()
{
	g_Position = vec4(
		VertexIn.WorldPosition,
		texture(sampler2D(textureArray.textures[VertexIn.TexIndex].AmbientOcclusion), VertexIn.TexCoord).r);

	g_Normal = vec4(
		computeNormal(), 
		texture(sampler2D(textureArray.textures[VertexIn.TexIndex].Roughness), VertexIn.TexCoord).r);

	g_AlbedoSpec = vec4(
		texture(sampler2D(textureArray.textures[VertexIn.TexIndex].Albedo), VertexIn.TexCoord).rgb, 
		texture(sampler2D(textureArray.textures[VertexIn.TexIndex].Metalness), VertexIn.TexCoord).r);
}

vec3 computeNormal()
{
	vec3 texNormal = texture(sampler2D(textureArray.textures[VertexIn.TexIndex].Normal), VertexIn.TexCoord).rgb * 2.0f - 1.0f;

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