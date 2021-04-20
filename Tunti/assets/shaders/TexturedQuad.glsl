#type vertex
#version 450 core

layout(location = 0) out vec2 v_TexCoord;

void main()
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);

    gl_Position = vec4(-1.0f + x*2.0f, -1.0f+y*2.0f, 0.0f, 1.0f);
    v_TexCoord = vec2(x, y);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

layout(location = 0) in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
    color = texture(u_Texture, v_TexCoord);
}