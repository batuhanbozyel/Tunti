#type vertex
#version 450 core

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 u_View;
    mat4 u_Projection;
};

out vec3 v_TexCoord;

void main()
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    mat4 view = mat4(mat3(u_View));
    vec4 pos = u_Projection * view * vec4(-1.0f + x*2.0f, -1.0f+y*2.0f, 0.0f, 1.0);
    gl_Position = pos.xyww;
    v_TexCoord = vec2(x, y);
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