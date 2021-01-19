#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Doge/Renderer/Texture.h"

namespace Doge
{
	struct Mesh;
	class MaterialInstance;

	struct Model
	{
		std::vector<Mesh> Meshes;
		std::vector<Ref<MaterialInstance>> MaterialInstances;
	};
	
	class ModelLibrary
	{
	public:
		static Ref<Model> Load(const std::string& filePath);
		static void Flush();
	private:
		static void ProcessNode(const std::string& directory, aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes);
		static Mesh ProcessMesh(const std::string& directory, aiMesh* mesh, const aiScene* scene);
		static void AddTexturePath(const std::string& directory,
			std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)>& texturePaths,
			aiMaterial* material, aiTextureType type, TextureType textureType);
	private:
		static std::unordered_map<std::string, Ref<Model>> s_ModelCache;
	};
}