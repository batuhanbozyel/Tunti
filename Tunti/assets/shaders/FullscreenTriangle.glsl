#type vertex
#version 450 core

layout(location = 0) out VertexInOut
{
    vec2 TexCoord;
} VertexOut;

void main()
{
    float x = -1.0f + float((gl_VertexID & 1) << 2);
	float y = -1.0f + float((gl_VertexID & 2) << 1);
    VertexOut.TexCoord = vec2((x + 1.0f) * 0.5f, (y + 1.0f) * 0.5f);
	gl_Position = vec4(x, -y, 0.0f, 1.0f);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

layout(location = 0) in VertexInOut
{
    vec2 TexCoord;
} VertexIn;

uniform sampler2D u_Texture;

void main()
{
    color = texture(u_Texture, VertexIn.TexCoord);
}