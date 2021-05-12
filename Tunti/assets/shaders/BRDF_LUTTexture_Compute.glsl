#version 450 core

const float PI = 3.141592;
const float TwoPI = 2 * PI;
const float Epsilon = 0.001;

const uint NumSamples = 1024;
const float InvNumSamples = 1.0 / float(NumSamples);

layout(binding = 0, rg16f) restrict writeonly uniform image2D LUT;

float RadicalInverse_VdC(uint bits);
vec2 SampleHammersley(uint i);
vec3 SampleGGX(float u1, float u2, float roughness);
float GeometryAttenuationSchlickG1(float cosTheta, float k);
float GeometryAttenuationSchlickGGX_IBL(float cosLi, float cosLo, float roughness);

layout(local_size_x=32, local_size_y=32, local_size_z=1) in;
void main()
{
	float cosLo = gl_GlobalInvocationID.x / float(imageSize(LUT).x);
	float roughness = gl_GlobalInvocationID.y / float(imageSize(LUT).y);

	cosLo = max(cosLo, Epsilon);

	vec3 Lo = vec3(sqrt(1.0 - cosLo*cosLo), 0.0, cosLo);

	float DFG1 = 0;
	float DFG2 = 0;

	for(uint i=0; i<NumSamples; ++i) 
	{
		vec2 u  = SampleHammersley(i);

		vec3 Lh = SampleGGX(u.x, u.y, roughness);

		vec3 Li = 2.0 * dot(Lo, Lh) * Lh - Lo;

		float cosLi   = Li.z;
		float cosLh   = Lh.z;
		float cosLoLh = max(dot(Lo, Lh), 0.0);

		if(cosLi > 0.0) 
		{
			float G  = GeometryAttenuationSchlickGGX_IBL(cosLi, cosLo, roughness);
			float Gv = G * cosLoLh / (cosLh * cosLo);
			float Fc = pow(1.0 - cosLoLh, 5);

			DFG1 += (1 - Fc) * Gv;
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

	float cosTheta = sqrt((1.0 - u2) / (1.0 + (alpha*alpha - 1.0) * u2));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	float phi = TwoPI * u1;

	return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

float GeometryAttenuationSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

float GeometryAttenuationSchlickGGX_IBL(float cosLi, float cosLo, float roughness)
{
	float r = roughness;
	float k = (r * r) / 2.0;
	return GeometryAttenuationSchlickG1(cosLi, k) * GeometryAttenuationSchlickG1(cosLo, k);
}