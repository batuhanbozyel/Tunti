#type vertex
#version 450 core
#define MAX_POINT_LIGHTS 100

layout(std430, binding = 2) readonly buffer VertexBuffer
{
	float data[];
} vertexBuffer;

struct DirectionalLight
{
    mat4 ViewProjection;
    vec4 DirectionAndIntensity;
    vec4 Color;
};

struct PointLight
{
    vec4 Position;
    vec4 Color;
    vec4 ConstantLinearQuadratic;
};

layout(std140, binding = 4) uniform LightsUniform
{
    DirectionalLight directionalLight;
    PointLight PointLights[MAX_POINT_LIGHTS];
    int NumPointLights;
};

uniform mat4 u_Model;

void main()
{
    uint posIndex = gl_VertexID * 3;
    gl_Position = directionalLight.ViewProjection * u_Model * vec4(
		vertexBuffer.data[posIndex], 
		vertexBuffer.data[posIndex + 1], 
		vertexBuffer.data[posIndex + 2], 1.0f);
}

#type fragment
#version 450 core

void main()
{

}