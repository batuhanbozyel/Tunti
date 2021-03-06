#pragma once

namespace Tunti
{
	struct Mesh;
	struct TextureMap;
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

	namespace ModelLibrary
	{
		Ref<Model> Load(const std::string& filePath);
		// TODO: Use hash table to retrieve path instead of linear searching
		std::string GetModelPath(const Ref<Model>& model);
		void Flush();
	};
}