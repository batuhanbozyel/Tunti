#pragma once
#include <string_view>

namespace Doge
{
	using namespace std::literals;

	// Textured Quad Shader

	static const auto TexturedQuadVertexShader = R"(
	#version 450 core

	layout(location = 0) in vec2  a_Position;
	layout(location = 1) in vec2  a_TexCoord;

	out vec2 v_TexCoord;

	void main()
	{
		v_TexCoord = a_TexCoord;
		gl_Position = vec4(a_Position, 0.0, 1.0);
	})"sv;

	static const auto TexturedQuadFragmentShader = R"(
	#version 450 core

	layout(location = 0) out vec4 color;

	in vec2 v_TexCoord;

	uniform sampler2D u_Texture;

	void main()
	{
		color = texture(u_Texture, v_TexCoord);
	})"sv;

	// Object Outlining Shader

	static const auto ObjectOutliningVertexShader = R"(
	#version 450 core

	layout(location = 0) in vec4  a_Position;
	layout(location = 1) in vec3  a_Normal;
	layout(location = 2) in vec2  a_TexCoord;
	layout(location = 3) in uint  a_TexIndex;

	layout(std140, binding = 1) uniform ViewProjectionUniform
	{
		mat4 u_View;
		mat4 u_Projection;
	};

	uniform mat4 u_Model;

	void main()
	{
		gl_Position = u_Projection * u_View * u_Model * a_Position;
	})"sv;

	static const auto ObjectOutliningFragmentShader = R"(
	#version 450 core
	
	layout(location = 0) out vec4 color;

	uniform vec3 u_OutlineColor;

	void main()
	{
		color = vec4(u_OutlineColor, 1.0);
	})"sv;

	// Skybox Shader

	static const auto SkyboxVertexShader = R"(
	#version 450 core

	layout(location = 0) in vec3 a_Position;

	layout(std140, binding = 1) uniform ViewProjectionUniform
	{
		mat4 u_View;
		mat4 u_Projection;
	};

	out vec3 v_TexCoord;

	void main()
	{
		v_TexCoord = -a_Position;
		mat4 view = mat4(mat3(u_View));
		vec4 pos = u_Projection * view * vec4(a_Position, 1.0);
		gl_Position = pos.xyww;
	})"sv;

	static const auto SkyboxFragmentShader = R"(
	#version 450 core
	
	layout(location = 0) out vec4 color;

	in vec3 v_TexCoord;

	uniform samplerCube u_Skybox;

	void main()
	{
		color = texture(u_Skybox, v_TexCoord);
	})"sv;
}