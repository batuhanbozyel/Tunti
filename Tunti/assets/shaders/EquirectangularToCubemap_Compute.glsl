#version 450 core

const float PI = 3.141592f;
const float TwoPI = 2.0f * PI;

layout(binding = 0) uniform sampler2D inputTexture;
layout(binding = 0, rgba16f) restrict writeonly uniform imageCube outputTexture;

vec3 GetSamplingVector();

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main()
{
	vec3 v = GetSamplingVector();

    // Convert Cartesian direction vector to spherical coordinates
	float phi   = atan(v.z, v.x);
	float theta = acos(v.y);

	vec4 color = texture(inputTexture, vec2(phi/TwoPI, theta/PI));

	imageStore(outputTexture, ivec3(gl_GlobalInvocationID), color);
}

vec3 GetSamplingVector()
{
    vec2 st = gl_GlobalInvocationID.xy / vec2(imageSize(outputTexture));
    vec2 uv = 2.0f * vec2(st.x, 1.0f - st.y) - vec2(1.0f);

    vec3 ret;
    if     (gl_GlobalInvocationID.z == 0) ret = vec3( 1.0f, uv.y, -uv.x);
    else if(gl_GlobalInvocationID.z == 1) ret = vec3(-1.0f, uv.y,  uv.x);
    else if(gl_GlobalInvocationID.z == 2) ret = vec3( uv.x, 1.0f, -uv.y);
    else if(gl_GlobalInvocationID.z == 3) ret = vec3( uv.x, -1.0f, uv.y);
    else if(gl_GlobalInvocationID.z == 4) ret = vec3( uv.x, uv.y,  1.0f);
    else if(gl_GlobalInvocationID.z == 5) ret = vec3(-uv.x, uv.y, -1.0f);
    return normalize(ret);
}