#pragma once
#include "Doge/Core/Base.h"

namespace Doge
{
	class VertexBuffer;
	class IndexBuffer;

	class Mesh;
	class Material;
	class MaterialInstance;

	struct MeshRendererData
	{
		std::vector<Ref<VertexBuffer>> VBOs;
		Ref<IndexBuffer> IBO;

		MeshRendererData(const Mesh& mesh);
		MeshRendererData(const std::vector<Mesh>& meshes);
	private:
		static Mesh BatchMeshes(const std::vector<Mesh>& meshes);
	};

	struct RenderData
	{
		MeshRendererData MeshData;
		Ref<MaterialInstance> MaterialInstanceRef;
		glm::mat4 Transform;

		RenderData(const MeshRendererData& meshData, const Ref<MaterialInstance>& material, const glm::mat4& transform)
			: MeshData(meshData), MaterialInstanceRef(material), Transform(transform) {}
	};
}