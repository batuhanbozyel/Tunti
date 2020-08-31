#include "pch.h"
#include "Model.h"

#include "Mesh.h"

#include "Doge/Renderer/Texture.h"
#include "Doge/Renderer/Renderer.h"

namespace Doge
{
	static constexpr unsigned int assimpFlags =
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_ValidateDataStructure |
		aiProcess_ImproveCacheLocality |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_FlipUVs;

	Model::Model(const std::string& filePath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath, assimpFlags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG_ERROR("ASSIMP: {0}", importer.GetErrorString());
			return;
		}
		m_Directory = filePath.substr(0, filePath.find_last_of('/') + 1);

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		uint32_t index = 0;

		// Process Material
		if (mesh->mMaterialIndex >= 0)
		{
			std::vector<std::pair<std::string, TextureType>> texturePaths;
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			AddTexturePath(texturePaths, material, aiTextureType_DIFFUSE, TextureType::Diffuse);
			AddTexturePath(texturePaths, material, aiTextureType_SPECULAR, TextureType::Specular);

			index = TextureManager::LoadTextureMaps(texturePaths);
		}

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			// Process Vertex Positions, Normals and Texture Coordinates
			glm::vec3 position;
			position.x = mesh->mVertices[i].x;
			position.y = mesh->mVertices[i].y;
			position.z = mesh->mVertices[i].z;

			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;

			glm::vec2 texCoord;
			if (mesh->mTextureCoords[0])
			{
				texCoord.x = mesh->mTextureCoords[0][i].x;
				texCoord.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				texCoord = glm::vec2(0.0f);
			}
			vertices.push_back(Vertex(position, normal, texCoord, index));
		}
		// Process Indices
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		return Mesh(vertices, indices);
	}

	void Model::AddTexturePath(std::vector<std::pair<std::string, TextureType>>& texturePaths, aiMaterial* material, aiTextureType type, TextureType textureType)
	{
		if (material->GetTextureCount(type))
		{
			aiString path;
			material->GetTexture(type, 0, &path);
			texturePaths.push_back(std::make_pair((m_Directory + path.C_Str()), textureType));
		}
	}

	void Model::Translate(const glm::vec3& pos)
	{
		for (auto& mesh : m_Meshes)
		{
			mesh.Translate(pos);
		}
	}

	void Model::Rotate(float angle, const glm::vec3& axis)
	{
		for (auto& mesh : m_Meshes)
		{
			mesh.Rotate(angle, axis);
		}
	}
}