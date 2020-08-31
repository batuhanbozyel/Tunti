#pragma once
#include "Mesh.h"

namespace Doge
{
	class SceneObject3D
	{
	public:
		friend class Renderer3D;
		virtual ~SceneObject3D() = default;

		virtual void Transform(const glm::mat4& transform) = 0;
	protected:
		virtual const std::vector<uint32_t> CalculateIndices(uint32_t& offset) = 0;

		inline const std::vector<Vertex>& GetVertices() const { return m_Vertices; }

		virtual inline uint32_t GetVertexSize() const = 0;
		virtual inline uint32_t GetIndexCount() const = 0;
	protected:
		std::vector<Vertex> m_Vertices;
	};

	class Cuboid : public SceneObject3D
	{
	public:
		explicit Cuboid(glm::vec3 length, uint32_t texIndex = 0);
		~Cuboid() = default;

		virtual void Transform(const glm::mat4& transform) override;
	private:
		virtual const std::vector<uint32_t> CalculateIndices(uint32_t& offset) override;

		virtual inline uint32_t GetVertexSize() const { return 24 * sizeof(Vertex); }
		virtual inline uint32_t GetIndexCount() const override { return 36; }
	};

	class Sphere : public SceneObject3D
	{
	public:
		explicit Sphere(float radius, uint32_t sectorCount = 18, uint32_t stackCount = 18, uint32_t texIndex = 0);
		~Sphere() = default;

		virtual void Transform(const glm::mat4& transform) override;
	private:
		virtual const std::vector<uint32_t> CalculateIndices(uint32_t& offset) override;

		virtual inline uint32_t GetVertexSize() const { return (uint32_t)m_Vertices.size() * sizeof(Vertex); }
		virtual inline uint32_t GetIndexCount() const { return m_IndexCount; }
	private:
		uint32_t m_IndexCount = 0;
		uint32_t m_SectorCount, m_StackCount;
	};
}