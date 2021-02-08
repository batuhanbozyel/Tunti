#type vertex
#version 450 core

layout(location = 0) out vec3 v_WorldPosition;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;
layout(location = 3) flat out uint v_TexIndex;

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 View;
    mat4 Projection;
	vec3 CameraPosition;
};

layout(std430, binding = 1) readonly buffer VertexBuffer
{
	float data[];
} vertexBuffer;

layout(std430, binding = 2) readonly buffer IndexBuffer
{
	uint data[];
} indexBuffer;

uniform uint u_VertexCount;

uniform mat4 u_Model;
uniform uint u_BaseVertex;

void main()
{
	uint index = indexBuffer.data[gl_VertexID] + u_BaseVertex;

	uint posIndex = index * 4;
	vec4 worldPosition = u_Model * vec4(
		vertexBuffer.data[posIndex], 
		vertexBuffer.data[posIndex + 1], 
		vertexBuffer.data[posIndex + 2], 
		vertexBuffer.data[posIndex + 3]);
	v_WorldPosition = vec3(worldPosition);

	uint normalIndex = u_VertexCount * 4 + index * 3;
	mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
	v_Normal = normalMatrix * vec3(
		vertexBuffer.data[normalIndex], 
		vertexBuffer.data[normalIndex + 1], 
		vertexBuffer.data[normalIndex + 2]);

	uint texCoordIndex = u_VertexCount * 7 + index * 2;
	v_TexCoord = vec2(
		vertexBuffer.data[texCoordIndex], 
		vertexBuffer.data[texCoordIndex + 1]);

	v_TexIndex = uint(vertexBuffer.data[u_VertexCount * 9 + index]);
	
	gl_Position = Projection * View * worldPosition;
}

#type fragment
#version 450 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec4 g_Position;
layout(location = 1) out vec4 g_Normal;
layout(location = 2) out vec4 g_AlbedoSpec;

layout(location = 0) in vec3 v_WorldPosition;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) flat in uint v_TexIndex;

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
		v_WorldPosition.xyz,
		texture(sampler2D(textureArray.textures[v_TexIndex].AmbientOcclusion), v_TexCoord).r);

	g_Normal = vec4(
		computeNormal().xyz, 
		texture(sampler2D(textureArray.textures[v_TexIndex].Roughness), v_TexCoord).r);

	g_AlbedoSpec = vec4(
		texture(sampler2D(textureArray.textures[v_TexIndex].Albedo), v_TexCoord).rgb, 
		texture(sampler2D(textureArray.textures[v_TexIndex].Metalness), v_TexCoord).r);
}

vec3 computeNormal()
{
	vec3 texNormal = normalize(texture(sampler2D(textureArray.textures[v_TexIndex].Normal), v_TexCoord).rgb * 2.0f - 1.0f);

	vec3 dPosX  = dFdx(v_WorldPosition);
    vec3 dPosY  = dFdy(v_WorldPosition);
    vec2 dTexX = dFdx(v_TexCoord);
    vec2 dTexY = dFdy(v_TexCoord);

    vec3 normal = normalize(v_Normal);
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, binormal, normal);

    return normalize(TBN * texNormal);
}