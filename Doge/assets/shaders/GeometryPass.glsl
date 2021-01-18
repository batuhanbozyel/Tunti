#type vertex
#version 450 core

layout(location = 0) in vec4  a_Position;
layout(location = 1) in vec3  a_Normal;
layout(location = 2) in vec2  a_TexCoord;
layout(location = 3) in uint  a_TexIndex;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
flat out uint v_TexIndex;

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_Position;
};

uniform mat4 u_Model;

void main()
{
	vec4 modelPosition = u_Model * a_Position;
	v_FragPos = vec3(modelPosition);
	v_Normal = a_Normal;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	gl_Position = u_Projection * u_View * modelPosition;
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
	uvec2 Specular;
};

layout(std430, binding = 2) readonly buffer TextureMaps
{
	TextureMap textures[];
} textureArray;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;
flat in uint v_TexIndex;

void main()
{
	g_Position = v_FragPos;
	g_Normal = normalize(v_Normal);
	g_AlbedoSpec.rgb = texture(sampler2D(textureArray.textures[v_TexIndex].Albedo), v_TexCoord).rgb;
	g_AlbedoSpec.a = texture(sampler2D(textureArray.textures[v_TexIndex].Specular), v_TexCoord).a;
}