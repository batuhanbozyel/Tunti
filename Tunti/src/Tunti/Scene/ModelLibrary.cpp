#include "pch.h"
#include "ModelLibrary.h"

#include "Tunti/Renderer/Material.h"
#include "Tunti/Renderer/Mesh.h"

#include <assimp/pbrmaterial.h>

namespace Tunti
{
	static constexpr unsigned int assimpFlags =
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_ImproveCacheLocality |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_FlipUVs;

	Ref<Model> ModelLibrary::Load(const std::string& filePath)
	{
		const auto& modelCacheIt = s_ModelCache.find(filePath);

		if (modelCacheIt == s_ModelCache.end())
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filePath, assimpFlags);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				Log::Error("ASSIMP: {0}", importer.GetErrorString());
				return nullptr;
			}
			std::string directory = filePath.substr(0, filePath.find_last_of('/') + 1);

			Ref<Model> model = CreateRef<Model>();
			ProcessNode(directory, scene->mRootNode, scene, model);

			const auto& insertLocationIt = s_ModelCache.insert(s_ModelCache.end(), { filePath, model });

			return insertLocationIt->second;
		}

		return modelCacheIt->second;
	}

	void ModelLibrary::Flush()
	{
		s_ModelCache.clear();
	}

	void ModelLibrary::ProcessNode(const std::string& directory, aiNode* node, const aiScene* scene, Ref<Model> model)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessModel(directory, mesh, scene, model);
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(directory, node->mChildren[i], scene, model);
		}
	}

	void ModelLibrary::ProcessModel(const std::string& directory, aiMesh* mesh, const aiScene* scene, Ref<Model> model)
	{
		Mesh subMesh;
		Ref<TextureMap> textureMap;

		// Process Material
		if (mesh->mMaterialIndex >= 0)
		{
			std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)> texturePaths;
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			AddTexturePath(directory, texturePaths, material, aiTextureType_DIFFUSE, TextureType::Albedo);
			if (texturePaths[static_cast<uint16_t>(TextureType::Albedo)].empty())
				AddTexturePath(directory, texturePaths, material, aiTextureType_BASE_COLOR, TextureType::Albedo);

			AddTexturePath(directory, texturePaths, material, aiTextureType_NORMALS, TextureType::Normal);
			if (texturePaths[static_cast<uint16_t>(TextureType::Normal)].empty())
				AddTexturePath(directory, texturePaths, material, aiTextureType_NORMAL_CAMERA, TextureType::Normal);
			if(texturePaths[static_cast<uint16_t>(TextureType::Normal)].empty())
				AddTexturePath(directory, texturePaths, material, aiTextureType_HEIGHT, TextureType::Normal);

			AddTexturePath(directory, texturePaths, material, aiTextureType_SPECULAR, TextureType::Metalness);
			if (texturePaths[static_cast<uint16_t>(TextureType::Metalness)].empty())
				AddTexturePath(directory, texturePaths, material, aiTextureType_METALNESS, TextureType::Metalness);

			AddTexturePath(directory, texturePaths, material, aiTextureType_SHININESS, TextureType::Roughness);
			if (texturePaths[static_cast<uint16_t>(TextureType::Roughness)].empty())
				AddTexturePath(directory, texturePaths, material, aiTextureType_DIFFUSE_ROUGHNESS, TextureType::Roughness);
			
			AddTexturePath(directory, texturePaths, material, aiTextureType_LIGHTMAP, TextureType::AmbientOcclusion);
			if (texturePaths[static_cast<uint16_t>(TextureType::AmbientOcclusion)].empty())
				AddTexturePath(directory, texturePaths, material, aiTextureType_AMBIENT_OCCLUSION, TextureType::AmbientOcclusion);

			textureMap = TextureLibrary::LoadTextureMap(texturePaths);
			model->TextureMaps.push_back(textureMap);
			model->MaterialInstances.push_back(Material::DefaulMaterialInstance());
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
			subMesh.Position.push_back(position);
			subMesh.Normal.push_back(normal);
			subMesh.TexCoord.push_back(texCoord);
			subMesh.TexIndex.push_back(textureMap->Index);
		}
		// Process Indices
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				subMesh.Indices.push_back(face.mIndices[j]);
		}

		model->Meshes.push_back(subMesh);
	}

	void ModelLibrary::AddTexturePath(const std::string& directory,
		std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)>& texturePaths,
		aiMaterial* material, aiTextureType type, TextureType textureType)
	{
		if (material->GetTextureCount(type))
		{
			aiString path;
			material->GetTexture(type, 0, &path);
			texturePaths[static_cast<uint16_t>(textureType)] = directory + path.C_Str();
		}
	}

	std::unordered_map<std::string, Ref<Model>> ModelLibrary::s_ModelCache;
}