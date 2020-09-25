#pragma once
#include "Doge/Core/Base.h"

namespace Doge
{
	class VertexBuffer;
	class IndexBuffer;

	class Mesh;
	class Material;
	class MaterialInstance;

	struct RenderData
	{
		RenderData(const std::vector<Ref<VertexBuffer>>& vertexBuffers,
			const Ref<IndexBuffer>& indexBuffer,
			const Ref<MaterialInstance>& matInstance)
			: VBOs(vertexBuffers), IBO(indexBuffer), MaterialInstanceRef(matInstance)
		{

		}

		std::vector<Ref<VertexBuffer>> VBOs;
		Ref<IndexBuffer> IBO;
		Ref<MaterialInstance> MaterialInstanceRef;
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		bool Selected = false;
	};

	class RenderDataManager
	{
	public:
		static RenderData Construct(const Mesh& mesh, const Ref<MaterialInstance>& materialInstance);
		static RenderData ConstructBatched(const std::vector<Mesh>& meshes, const Ref<MaterialInstance>& materialInstance);
	private:
		static Mesh BatchMeshes(const std::vector<Mesh>& meshes);
	};
}