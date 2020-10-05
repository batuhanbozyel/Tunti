#include "pch.h"
#include "RenderDataManager.h"

#include "Buffer.h"
#include "ShaderDataType.h"

#include "Doge/Utility/Mesh.h"

namespace Doge
{
	RenderData RenderDataManager::ConstructBatched(const std::vector<Mesh>& meshes, const Ref<MaterialInstance>& materialInstance)
	{
		return Construct(BatchMeshes(meshes), materialInstance);
	}

	RenderData RenderDataManager::Construct(const Mesh& mesh, const Ref<MaterialInstance>& materialInstance)
	{
		std::vector<Ref<VertexBuffer>> vertexBuffers;
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(&mesh.GetVertices().data()->Position.x,
															  static_cast<uint32_t>(mesh.GetVertices().size() * sizeof(Vertex)));

		vertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal"   },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::UInt,   "a_TexIndex" }
		});
		vertexBuffers.push_back(vertexBuffer);

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(mesh.GetIndices().data(), static_cast<uint32_t>(mesh.GetIndices().size()));

		return RenderData(std::move(vertexBuffers), std::move(indexBuffer), materialInstance);
	}

	Mesh RenderDataManager::BatchMeshes(const std::vector<Mesh>& meshes)
	{
		uint32_t offset = 0;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		for (const Mesh& mesh : meshes)
		{
			auto meshVertices = mesh.GetVertices();
			vertices.insert(std::end(vertices), std::begin(meshVertices), std::end(meshVertices));

			auto meshIndices = mesh.GetIndices();
			std::for_each(meshIndices.begin(), meshIndices.end(), [&offset = std::as_const(offset)](uint32_t& indice)
			{
				indice += offset;
			});

			indices.insert(std::end(indices), std::begin(meshIndices), std::end(meshIndices));

			offset += static_cast<uint32_t>(meshVertices.size());
		}
		return Mesh(vertices, indices);
	}

}