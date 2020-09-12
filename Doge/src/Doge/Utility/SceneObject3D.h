#pragma once
#include "Mesh.h"

namespace Doge
{
	class SceneObject3D
	{
	public:
		virtual ~SceneObject3D() = default;

		inline const Mesh& GetMesh() const { return m_Mesh; }
	protected:
		virtual const std::vector<uint32_t> CalculateIndices() = 0;
	protected:
		Mesh m_Mesh;
	};

	class Cuboid : public SceneObject3D
	{
	public:
		explicit Cuboid(glm::vec3 length, uint32_t texIndex = 0);
		~Cuboid() = default;
	private:
		virtual const std::vector<uint32_t> CalculateIndices() override;
	};

	class Sphere : public SceneObject3D
	{
	public:
		explicit Sphere(float radius, uint32_t sectorCount = 18, uint32_t stackCount = 18, uint32_t texIndex = 0);
		~Sphere() = default;
	private:
		virtual const std::vector<uint32_t> CalculateIndices() override;
	private:
		uint32_t m_IndexCount = 0;
		uint32_t m_SectorCount, m_StackCount;
	};
}