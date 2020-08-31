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
			const std::shared_ptr<Material>& mat,
			const glm::mat4& model = glm::mat4(1.0f))
			: VBOs(vertexBuffers), IBO(indexBuffer), material(mat), modelMatrix(model)
		{

		}

		std::vector<std::shared_ptr<VertexBuffer>> VBOs;
		std::shared_ptr<IndexBuffer> IBO;
		std::shared_ptr<Material> material;
		glm::mat4 modelMatrix;
	};

	class RenderDataManager
	{
	public:
		static RenderData ConstructBatched(const std::vector<Mesh>& meshes, const std::shared_ptr<Material>& material, const glm::mat4& model = glm::mat4(1.0f));
		static RenderData Construct(const Mesh& mesh, const std::shared_ptr<Material>& material, const glm::mat4& model = glm::mat4(1.0f));
	private:
		static Mesh BatchMeshes(const std::vector<Mesh>& meshes);
	};
}