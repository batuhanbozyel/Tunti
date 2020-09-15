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
			: VBOs(vertexBuffers), IBO(indexBuffer), MaterialRef(mat)
		{

		}

		std::vector<std::shared_ptr<VertexBuffer>> VBOs;
		std::shared_ptr<IndexBuffer> IBO;
		std::shared_ptr<Material> MaterialRef;
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		bool Selected = false;
	};

	class RenderDataManager
	{
	public:
		static RenderData Construct(const Mesh& mesh, const std::shared_ptr<Material>& material);
		static RenderData ConstructBatched(const std::vector<Mesh>& meshes, const std::shared_ptr<Material>& material);

// 		static RenderData ConstructInstanced(const Mesh& mesh, const std::shared_ptr<Material>& material);
// 		static RenderData ConstructBatchedInstanced(const std::vector<Mesh>& meshes, const std::shared_ptr<Material>& material);
	private:
		static Mesh BatchMeshes(const std::vector<Mesh>& meshes);
	};
}