#pragma once

namespace Doge
{
	class VertexBuffer;
	class IndexBuffer;

	class Mesh;
	class Material;

	struct RenderData
	{
		RenderData(const std::vector<std::shared_ptr<VertexBuffer>>& vertexBuffers,
			const std::shared_ptr<IndexBuffer>& indexBuffer,
			const std::shared_ptr<Material>& mat)
			: VBOs(vertexBuffers), IBO(indexBuffer), material(mat)
		{

		}

		std::vector<std::shared_ptr<VertexBuffer>> VBOs;
		std::shared_ptr<IndexBuffer> IBO;
		std::shared_ptr<Material> material;
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		bool Selected = false;
	};

	class RenderDataManager
	{
	public:
		static RenderData ConstructBatched(const std::vector<Mesh>& meshes, const std::shared_ptr<Material>& material);
		static RenderData Construct(const Mesh& mesh, const std::shared_ptr<Material>& material);
	private:
		static Mesh BatchMeshes(const std::vector<Mesh>& meshes);
	};
}