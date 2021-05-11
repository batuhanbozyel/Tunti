#type vertex
#version 450 core

layout(std430, binding = 2) readonly buffer VertexBuffer
{
	float data[];
} vertexBuffer;

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main()
{
    uint posIndex = gl_VertexID * 3;
    gl_Position = u_LightSpaceMatrix * u_Model * vec4(
		vertexBuffer.data[posIndex], 
		vertexBuffer.data[posIndex + 1], 
		vertexBuffer.data[posIndex + 2], 1.0);
}

#type fragment
#version 450 core

void main()
{

}