#type vertex
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
    v_TexCoord = a_Position;
    mat4 view = mat4(mat3(u_View));
    vec4 pos = u_Projection * view * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}

#type fragment
#version 450 core
	
layout(location = 0) out vec4 color;

in vec3 v_TexCoord;

uniform samplerCube u_Skybox;

void main()
{
    color = texture(u_Skybox, v_TexCoord);
}