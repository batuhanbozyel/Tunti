#type vertex
#version 450 core

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 View;
	mat4 Projection;
	vec3 CameraPosition;
};

layout(location = 0) out vec3 v_WorldPos;

void main()
{
    uint b = 1 << gl_VertexID;
    vec3 pos = vec3((0x287a & b) != 0, (0x02af & b) != 0, (0x31e3 & b) != 0) - vec3(0.5f);
    gl_Position = Projection * View * vec4(pos + CameraPosition, 1.0f);
    v_WorldPos = pos;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 v_WorldPos;

uniform samplerCube u_Skybox;

void main()
{
    color = textureLod(u_Skybox, v_WorldPos, 0);
}