#type vertex
#version 450 core

layout(location = 0) in vec4  a_Position;

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_Position;
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