#type vertex
#version 450 core

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 u_View;
    mat4 u_Projection;
};

layout(location = 0) out vec3 v_TexCoord;

void main()
{
    uint b = 1 << gl_VertexID;
    vec3 pos = vec3((0x287a & b) != 0, (0x02af & b) != 0, (0x31e3 & b) != 0);

    mat4 view = mat4(mat3(u_View));
    vec4 clipPos = u_Projection * view * vec4(pos, 1.0f);

    gl_Position = clipPos.xyww;
    v_TexCoord = pos;
}

#type fragment
#version 450 core
	
layout(location = 0) out vec4 color;

layout(location = 0) in vec3 v_TexCoord;

uniform samplerCube u_Skybox;

void main()
{
    color = textureLod(u_Skybox, v_TexCoord, 0);
}