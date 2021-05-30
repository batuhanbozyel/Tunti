#pragma once
#include "Tunti/Renderer/BufferManager.h"

namespace Tunti
{
	struct MaterialInstance;

	enum class PrimitiveMesh
	{
		CUBE,
		SPHERE,
		CONE,
		CYLINDER,
		PLANE,
		PRIMITIVE_MESH_COUNT
	};

	struct Model
	{
		MeshBuffer _Mesh;
		std::vector<SubmeshBuffer> Submeshes;
		std::vector<Ref<MaterialInstance>> Materials;

		Model() = default;
		Model(const MeshBuffer& mesh, const std::vector<SubmeshBuffer>& submeshes, const std::vector<Ref<MaterialInstance>>& materials)
			: _Mesh(mesh), Submeshes(submeshes), Materials(materials) {}
	};

	class ModelLibrary final
	{
	public:
		static Ref<Model> Load(const std::string& filePath);
		static Ref<Model> LoadPrimitive(PrimitiveMesh primitive);
		static std::string GetModelPath(const Ref<Model>& model);
		static void Flush();
	};
}