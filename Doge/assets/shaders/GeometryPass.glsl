#type vertex
#version 450 core

out vec3 v_WorldPosition;
out vec3 v_Normal;
out vec2 v_TexCoord;
flat out uint v_TexIndex;

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
		vertexBuffer.data[texCoordIndex+ 1]);

	v_TexIndex = uint(vertexBuffer.data[u_VertexCount * 9 + index]);

	gl_Position = Projection * View * worldPosition;
}

#type fragment
#version 450 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec3 g_Position;
layout(location = 1) out vec3 g_Normal;
layout(location = 2) out vec4 g_AlbedoSpec;

struct TextureMap
{
	uvec2 Albedo;
	uvec2 Normal;
	uvec2 Metallic;
	uvec2 Roughness;
	uvec2 AmbientOcclusion;
};

layout(std430, binding = 3) readonly buffer TextureMaps
{
	TextureMap textures[];
} textureArray;

in vec3 v_WorldPosition;
in vec3 v_Normal;
in vec2 v_TexCoord;
flat in uint v_TexIndex;

void main()
{
	g_Position = v_WorldPosition;
	g_Normal = normalize(v_Normal);
	g_AlbedoSpec.rgb = texture(sampler2D(textureArray.textures[v_TexIndex].Albedo), v_TexCoord).rgb;
	g_AlbedoSpec.a = texture(sampler2D(textureArray.textures[v_TexIndex].Metallic), v_TexCoord).r;
}