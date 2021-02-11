#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Tunti/Renderer/Texture.h"

namespace Tunti
{
	struct Mesh;
	class MaterialInstance;

	struct Model
	{
		std::vector<Mesh> Meshes;
		std::vector<Ref<TextureMap>> TextureMaps;
		std::vector<Ref<MaterialInstance>> MaterialInstances;

		Model() = default;

		Model(const std::vector<Mesh>& meshes, const std::vector<Ref<MaterialInstance>> materialInstances)
			: Meshes(meshes), MaterialInstances(materialInstances) {}
	};

	class ModelLibrary final
	{
	public:
		static Ref<Model> Load(const std::string& filePath);
		static void Flush();
	private:
		static void ProcessNode(const std::string& directory, aiNode* node, const aiScene* scene, Ref<Model> model);
		static void ProcessModel(const std::string& directory, aiMesh* mesh, const aiScene* scene, Ref<Model> model);
		static void AddTexturePath(const std::string& directory,
			std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)>& texturePaths,
			aiMaterial* material, aiTextureType type, TextureType textureType);
	private:
		static std::unordered_map<std::string, Ref<Model>> s_ModelCache;
	};
}