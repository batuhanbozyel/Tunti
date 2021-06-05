#version 450 core

const float PI = 3.141592f;
const float TwoPI = 2.0f * PI;
const float Epsilon = 1e-5f;

const uint NumSamples = 1024;
const float InvNumSamples = 1.0f / float(NumSamples);

layout(binding = 0, rg32f) restrict writeonly uniform image2D LUT;

float RadicalInverse_VdC(uint bits);
vec2 SampleHammersley(uint i);
vec3 SampleGGX(float u1, float u2, float roughness);
float GeometrySchlickGGX(float cosTheta, float k);
float GeometrySmith(float cosLi, float cosLo, float roughness);

layout(local_size_x=32, local_size_y=32, local_size_z=1) in;
void main()
{
	float cosLo = gl_GlobalInvocationID.x / float(imageSize(LUT).x);
	float roughness = gl_GlobalInvocationID.y / float(imageSize(LUT).y);

	cosLo = max(cosLo, Epsilon);

	vec3 Lo = vec3(sqrt(1.0f - cosLo * cosLo), 0.0f, cosLo);

	float DFG1 = 0.0f;
	float DFG2 = 0.0f;

	for(uint i=0; i<NumSamples; ++i) 
	{
		vec2 u  = SampleHammersley(i);

		vec3 Lh = SampleGGX(u.x, u.y, roughness);

		vec3 Li = 2.0f * dot(Lo, Lh) * Lh - Lo;

		float cosLi   = Li.z;
		float cosLh   = Lh.z;
		float cosLoLh = max(dot(Lo, Lh), 0.0f);

		if(cosLi > 0.0) 
		{
			float G  = GeometrySmith(cosLi, cosLo, roughness);
			float Gv = G * cosLoLh / (cosLh * cosLo);
			float Fc = pow(1.0f - cosLoLh, 5);

			DFG1 += (1.0f - Fc) * Gv;
			DFG2 += Fc * Gv;
		}
	}

	imageStore(LUT, ivec2(gl_GlobalInvocationID), vec4(DFG1, DFG2, 0, 0) * InvNumSamples);
}

float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 SampleHammersley(uint i)
{
	return vec2(i * InvNumSamples, RadicalInverse_VdC(i));
}

vec3 SampleGGX(float u1, float u2, float roughness)
{
	float alpha = roughness * roughness;

	float cosTheta = sqrt((1.0f - u2) / (1.0f + (alpha * alpha - 1.0f) * u2));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
	float phi = TwoPI * u1;

	return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

// Single term for separable Schlick-GGX below.
float GeometrySchlickGGX(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0f - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float GeometrySmith(float cosLi, float cosLo, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    return GeometrySchlickGGX(cosLi, k) * GeometrySchlickGGX(cosLo, k);
}