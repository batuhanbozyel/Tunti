#include "pch.h"
#include "RenderDataManager.h"

#include "Buffer.h"

#include "Doge/Utility/Mesh.h"

namespace Doge
{
	RenderData RenderDataManager::ConstructBatched(const std::vector<Mesh>& meshes, const std::shared_ptr<Material>& material, const glm::mat4& model)
	{
		return Construct(BatchMeshes(meshes), material, model);
	}

	RenderData RenderDataManager::Construct(const Mesh& mesh, const std::shared_ptr<Material>& material, const glm::mat4& model)
	{
		std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
		std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(&mesh.GetVertices().data()->Position.x,
			static_cast<uint32_t>(mesh.GetVertices().size() * sizeof(Vertex)));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::UInt , "a_TexIndex" }
			});
		vertexBuffers.push_back(vertexBuffer);

		std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(mesh.GetIndices().data(),
			static_cast<uint32_t>(mesh.GetIndices().size()));

		return RenderData(vertexBuffers, indexBuffer, material, model);
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
			std::for_each(meshIndices.begin(), meshIndices.end(), [&](uint32_t& indice)
				{
					indice += offset;
				});

			indices.insert(std::end(indices), std::begin(meshIndices), std::end(meshIndices));

			offset += static_cast<uint32_t>(meshVertices.size());
		}
		return Mesh(vertices, indices);
	}

}