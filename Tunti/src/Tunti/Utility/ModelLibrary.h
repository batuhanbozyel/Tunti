#pragma once
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

	namespace ModelLibrary
	{
		Ref<Model> Load(const std::string& filePath);
		void Flush();
	};
}