#type vertex
#version 450 core

layout(std140, binding = 0) uniform ViewProjectionUniform
{
    mat4 ViewProjection;
	vec3 CameraPosition;
};

layout(std430, binding = 2) readonly buffer VertexBuffer
{
	float data[];
} vertexBuffer;

void main()
{
    uint posIndex = gl_VertexID * 3;
    gl_Position = ViewProjection * vec4(
		vertexBuffer.data[posIndex], 
		vertexBuffer.data[posIndex + 1], 
		vertexBuffer.data[posIndex + 2], 1.0);
}

#type fragment
#version 450 core

void main()
{
    
}