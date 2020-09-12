#pragma once

namespace Doge
{
	// Textured Quad Shader

	static const char* TexturedQuadVertexShader = R"(
	#version 450 core

	layout(location = 0) in vec2  a_Position;
	layout(location = 1) in vec2  a_TexCoord;

	out vec2 v_TexCoord;

	void main()
	{
		v_TexCoord = a_TexCoord;
		gl_Position = vec4(a_Position, 0.0, 1.0);
	})";

	static const char* TexturedQuadFragmentShader = R"(
	#version 450 core

	layout(location = 0) out vec4 color;

	in vec2 v_TexCoord;

	uniform sampler2D u_Texture;

	void main()
	{
		color = texture(u_Texture, v_TexCoord);
	})";

	// Object Outlining Shader

	static const char* ObjectOutliningVertexShader = R"(
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
	})";

	static const char* ObjectOutliningFragmentShader = R"(
	#version 450 core
	
	layout(location = 0) out vec4 color;

	void main()
	{
		color = vec4(0.8, 0.5, 0.2, 1.0);
	})";
}