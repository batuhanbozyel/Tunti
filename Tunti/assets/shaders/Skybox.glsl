#type vertex
#version 450 core

layout(std140, binding = 0) uniform CameraUniform
{
    mat4 View;
	mat4 Projection;
	mat4 InvView;
	mat4 InvProjection;
	vec3 Position;
} Camera;

layout(location = 0) out VertexInOut
{
    vec3 WorldPosition;
} VertexOut;

void main()
{
    uint b = 1 << gl_VertexID;
    vec3 pos = vec3((0x287a & b) != 0, (0x02af & b) != 0, (0x31e3 & b) != 0);
    pos = pos - vec3(0.5f);
    gl_Position = Camera.Projection * Camera.View * vec4(pos + Camera.Position, 1.0f);
    VertexOut.WorldPosition = pos;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 outColor;

layout(location = 0) in VertexInOut
{
    vec3 WorldPosition;
} VertexIn;

uniform samplerCube u_Skybox;

void main()
{
    vec3 color = texture(u_Skybox, VertexIn.WorldPosition).rgb;

    // HDR tonemapping
    color = color / (color + vec3(1.0f));
    // gamma correction
    color = pow(color, vec3(1.0f / 2.2f));

    outColor = vec4(color, 1.0f);
}