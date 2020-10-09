#type vertex
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
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

uniform vec3 u_OutlineColor;

void main()
{
    color = vec4(u_OutlineColor, 1.0);
}