#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Doge
{
	struct Mesh;
	class MaterialInstance;

	enum class TextureType : uint16_t;

	struct Model
	{
		std::vector<Mesh> Meshes;
		std::vector<Ref<MaterialInstance>> MaterialInstances;
	};
	
	class ModelLibrary
	{
	public:
		static const Ref<Model>& Load(const std::string& filePath);
		static void Flush();
	private:
		static void ProcessNode(const std::string& directory, aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes);
		static Mesh ProcessMesh(const std::string& directory, aiMesh* mesh, const aiScene* scene);
		static void AddTexturePath(const std::string& directory,
			std::vector<std::tuple<std::string, TextureType>>& texturePaths,
			aiMaterial* material, aiTextureType type, TextureType textureType);
	private:
		static std::unordered_map<std::string, Ref<Model>> s_ModelCache;
	};
}