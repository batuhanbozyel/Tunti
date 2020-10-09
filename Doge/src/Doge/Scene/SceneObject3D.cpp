#include "pch.h"
#include "SceneObject3D.h"

#include "Doge/Renderer/Renderer.h"
#include "Doge/Renderer/Texture.h"

namespace Doge
{
	// Cuboid Object Implementations

	Cube::Cube(glm::vec3&& length, const Ref<Texture>& texture)
	{
		uint32_t texIndex = texture ? texture->GetShaderStorageIndex() : 0;

		std::vector<Vertex> vertices(24);
		length = length / 2.0f;

		// Right	
		vertices[0] = { glm::vec3(length.x, -length.y,  length.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), texIndex };
		vertices[1] = { glm::vec3(length.x, -length.y, -length.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), texIndex };
		vertices[2] = { glm::vec3(length.x,  length.y, -length.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), texIndex };
		vertices[3] = { glm::vec3(length.x,  length.y,  length.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Left
		vertices[4] =  { glm::vec3(-length.x, -length.y, -length.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), texIndex };
		vertices[5] =  { glm::vec3(-length.x, -length.y,  length.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), texIndex };
		vertices[6] = { glm::vec3(-length.x,  length.y,  length.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), texIndex };
		vertices[7] = { glm::vec3(-length.x,  length.y, -length.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Top
		vertices[8] = { glm::vec3(-length.x,  length.y,  length.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), texIndex };
		vertices[9] = { glm::vec3( length.x,  length.y,  length.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), texIndex };
		vertices[10] = { glm::vec3( length.x,  length.y, -length.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), texIndex };
		vertices[11] = { glm::vec3(-length.x,  length.y, -length.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Bottom
		vertices[12] = { glm::vec3(-length.x, -length.y, -length.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), texIndex };
		vertices[13] = { glm::vec3( length.x, -length.y, -length.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), texIndex };
		vertices[14] = { glm::vec3( length.x, -length.y,  length.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), texIndex };
		vertices[15] = { glm::vec3(-length.x, -length.y,  length.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Front
		vertices[16] = { glm::vec3(-length.x, -length.y,  length.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), texIndex };
		vertices[17] = { glm::vec3( length.x, -length.y,  length.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), texIndex };
		vertices[18] = { glm::vec3( length.x,  length.y,  length.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), texIndex };
		vertices[19] = { glm::vec3(-length.x,  length.y,  length.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Back
		vertices[20] = { glm::vec3( length.x, -length.y, -length.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), texIndex };
		vertices[21] = { glm::vec3(-length.x, -length.y, -length.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), texIndex };
		vertices[22] = { glm::vec3(-length.x,  length.y, -length.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), texIndex };
		vertices[23] = { glm::vec3( length.x,  length.y, -length.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), texIndex };

		m_Mesh.SetVertices(vertices);
		const auto& indices = CalculateIndices();
		m_Mesh.SetIndices(std::vector<uint32_t>(indices.begin(), indices.end()));
	}

	std::array<glm::vec3, 24> Cube::CalculatePositions(glm::vec3&& length)
	{
		std::array<glm::vec3, 24> vertices;
		length = length / 2.0f;

		// Right	
		vertices[0] =  { glm::vec3(length.x, -length.y,  length.z) };
		vertices[1] =  { glm::vec3(length.x, -length.y, -length.z) };
		vertices[2] =  { glm::vec3(length.x,  length.y, -length.z) };
		vertices[3] =  { glm::vec3(length.x,  length.y,  length.z) };
		// Left		   
		vertices[4] =  { glm::vec3(-length.x, -length.y, -length.z) };
		vertices[5] =  { glm::vec3(-length.x, -length.y,  length.z) };
		vertices[6] =  { glm::vec3(-length.x,  length.y,  length.z) };
		vertices[7] =  { glm::vec3(-length.x,  length.y, -length.z) };
		// Top
		vertices[8] =  { glm::vec3(-length.x,  length.y,  length.z) };
		vertices[9] =  { glm::vec3( length.x,  length.y,  length.z) };
		vertices[10] = { glm::vec3( length.x,  length.y, -length.z) };
		vertices[11] = { glm::vec3(-length.x,  length.y, -length.z) };
		// Bottom
		vertices[12] = { glm::vec3(-length.x, -length.y, -length.z) };
		vertices[13] = { glm::vec3( length.x, -length.y, -length.z) };
		vertices[14] = { glm::vec3( length.x, -length.y,  length.z) };
		vertices[15] = { glm::vec3(-length.x, -length.y,  length.z) };
		// Front
		vertices[16] = { glm::vec3(-length.x, -length.y,  length.z) };
		vertices[17] = { glm::vec3( length.x, -length.y,  length.z) };
		vertices[18] = { glm::vec3( length.x,  length.y,  length.z) };
		vertices[19] = { glm::vec3(-length.x,  length.y,  length.z) };
		// Back
		vertices[20] = { glm::vec3( length.x, -length.y, -length.z) };
		vertices[21] = { glm::vec3(-length.x, -length.y, -length.z) };
		vertices[22] = { glm::vec3(-length.x,  length.y, -length.z) };
		vertices[23] = { glm::vec3( length.x,  length.y, -length.z) };

		return vertices;
	}

	std::array<uint32_t, 36> Cube::CalculateIndices()
	{
		std::array<uint32_t, 36> indices;
		uint32_t offset = 0;
		for (uint32_t i = 0; i <= 30; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}
		return indices;
	}


	// Sphere Object Implementations

	Sphere::Sphere(float radius, uint32_t sectorCount, uint32_t stackCount, const Ref<Texture>& texture)
	{
		uint32_t texIndex = texture ? texture->GetShaderStorageIndex() : 0;

		std::vector<Vertex> vertices;
		radius = radius / 2.0f;
		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / radius;		// vertex normal
		float s, t;                                     // vertex texCoord

		float sectorCounttep = 2 * glm::pi<float>() / sectorCount;
		float stackStep = glm::pi<float>() / stackCount;
		float sectorAngle, stackAngle;

		for (uint32_t i = 0; i <= stackCount; i++)
		{
			stackAngle = i * stackStep - glm::pi<float>() / 2;		// starting from -pi/2 to pi/2
			xy = radius * glm::cos(stackAngle);
			z = radius * glm::sin(stackAngle);

			for (uint32_t j = 0; j <= sectorCount; j++)
			{
				sectorAngle = j * sectorCounttep;		// starting from 0 to 2pi

				x = xy * glm::cos(sectorAngle);
				y = xy * glm::sin(sectorAngle);
				glm::vec3 pos(x, y, z);

				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				glm::vec3 normal(nx, ny, nz);

				s = (float)j / sectorCount;
				t = (float)i / stackCount;
				glm::vec2 texCoord(s, t);

				vertices.push_back(Vertex(pos, normal, texCoord, texIndex));
			}
		}
		glm::mat4 transform(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
		for (auto& vertex : vertices)
		{
			vertex.Position = transform * vertex.Position;
			vertex.Normal = transform * glm::vec4(vertex.Normal, 0.0f);
		}
		m_Mesh.SetVertices(vertices);
		m_Mesh.SetIndices(CalculateIndices(sectorCount, stackCount));
	}

	std::vector<uint32_t> Sphere::CalculateIndices(uint32_t sectorCount, uint32_t stackCount)
	{
		std::vector<uint32_t> indices;
		int k1, k2;
		uint32_t offset = 0;
		for (uint32_t i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);     // beginning of current stack
			k2 = k1 + sectorCount + 1;      // beginning of next stack

			for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(offset + k1);
					indices.push_back(offset + k2);
					indices.push_back(offset + k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (stackCount - 1))
				{
					indices.push_back(offset + k1 + 1);
					indices.push_back(offset + k2);
					indices.push_back(offset + k2 + 1);
				}
			}
		}
		uint32_t verticeCount = (stackCount + 1) * (sectorCount + 1);
		offset += verticeCount;
		return indices;
	}
}