#pragma once
#include "Doge/Core/Base.h"

namespace Doge
{
	class VertexBuffer;
	class IndexBuffer;

	class Mesh;
	class Material;

	struct RenderData
	{
		RenderData(const std::vector<Ref<VertexBuffer>>& vertexBuffers,
			const Ref<IndexBuffer>& indexBuffer,
			const Ref<Material>& mat)
			: VBOs(vertexBuffers), IBO(indexBuffer), MaterialRef(mat)
		{

		}

		std::vector<Ref<VertexBuffer>> VBOs;
		Ref<IndexBuffer> IBO;
		Ref<Material> MaterialRef;
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		bool Selected = false;
	};

	class RenderDataManager
	{
	public:
		static RenderData Construct(const Mesh& mesh, const Ref<Material>& material);
		static RenderData ConstructBatched(const std::vector<Mesh>& meshes, const Ref<Material>& material);
	private:
		static Mesh BatchMeshes(const std::vector<Mesh>& meshes);
	};
}