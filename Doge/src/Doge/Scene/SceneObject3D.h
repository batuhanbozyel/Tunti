#pragma once
#include "Doge/Utility/Mesh.h"

namespace Doge
{
	class Texture;
	class CubemapTexture;

	class SceneObject3D
	{
	public:
		virtual ~SceneObject3D() = default;

		inline const Mesh& GetMesh() const { return m_Mesh; }
	protected:
		Mesh m_Mesh;
	};

	class Cube : public SceneObject3D
	{
	public:
		explicit Cube(glm::vec3&& length, const Ref<Texture>& texture = nullptr);
		~Cube() = default;

		static std::array<glm::vec3, 24> CalculatePositions(glm::vec3&& length);
		static std::array<uint32_t, 36> CalculateIndices();
	};

	class Sphere : public SceneObject3D
	{
	public:
		explicit Sphere(float radius, uint32_t sectorCount = 18, uint32_t stackCount = 18, const Ref<Texture>& texture = nullptr);
		~Sphere() = default;
	private:
		static std::vector<uint32_t> CalculateIndices(uint32_t sectorCount, uint32_t stackCount);
	};
}