#type vertex
#version 450 core

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 View;
	mat4 Projection;
	vec3 CameraPosition;
};

layout(location = 0) out VertexInOut
{
    vec3 WorldPos;
} VertexOut;

void main()
{
    uint b = 1 << gl_VertexID;
    vec3 pos = vec3((0x287a & b) != 0, (0x02af & b) != 0, (0x31e3 & b) != 0) - vec3(0.5f);
    gl_Position = Projection * View * vec4(pos + CameraPosition, 1.0f);
    VertexOut.WorldPos = pos;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 outColor;

layout(location = 0) in VertexInOut
{
    vec3 WorldPos;
} VertexIn;

uniform samplerCube u_Skybox;

void main()
{
    vec3 color = texture(u_Skybox, VertexIn.WorldPos).rgb;

    // HDR tonemapping
    color = color / (color + vec3(1.0f));
    // gamma correction
    color = pow(color, vec3(1.0f / 2.2f));

    outColor = vec4(color, 1.0f);
}