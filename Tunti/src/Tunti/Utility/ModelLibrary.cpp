#include "pch.h"
#include "ModelLibrary.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#include "Tunti/Renderer/Material.h"
#include "Tunti/Renderer/Texture.h"
#include "Tunti/Renderer/Mesh.h"

namespace Tunti
{
	namespace ObjLoader
	{
		static Ref<Model> Load(const std::string& filePath)
		{
			std::string fileDirectory = std::filesystem::path(filePath).parent_path().string().append("/");
			tinyobj::ObjReaderConfig reader_config;
			tinyobj::ObjReader reader;
			if (!reader.ParseFromFile(filePath, reader_config))
			{
				if (!reader.Error().empty())
					Log::Error("TinyObjReader: {0}", reader.Error());

				return nullptr;
			}

			if (!reader.Warning().empty())
				Log::Warn("TinyObjReader:  {0}", reader.Warning());

			auto& attrib = reader.GetAttrib();
			auto& shapes = reader.GetShapes();
			auto& materials = reader.GetMaterials();

			Ref<Model> model = CreateRef<Model>();
			Ref<Material> pbrMaterial = MaterialLibrary::PBRMaterial();
			model->_Mesh = BufferManager::AllocateMeshBufferWithKey(pbrMaterial->Index);
			std::unordered_map<Vertex, uint32_t> uniqueVertices{};
			for (const tinyobj::shape_t& shape : shapes)
			{
				Submesh submesh;

				bool shouldCreateNewMaterialInstance = false;
				std::array<std::string, static_cast<uint32_t>(PBRTextureMap::COUNT)> texturePaths;
				if (materials.size())
				{
					// Albedo
					if (!materials[shape.mesh.material_ids[0]].diffuse_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Albedo)] = fileDirectory + materials[shape.mesh.material_ids[0]].diffuse_texname;
						shouldCreateNewMaterialInstance = true;
					}

					// Normal
					if (!materials[shape.mesh.material_ids[0]].normal_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Normal)] = fileDirectory + materials[shape.mesh.material_ids[0]].normal_texname;
						shouldCreateNewMaterialInstance = true;
					}
					else if (!materials[shape.mesh.material_ids[0]].bump_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Normal)] = fileDirectory + materials[shape.mesh.material_ids[0]].bump_texname;
						shouldCreateNewMaterialInstance = true;
					}

					// Metalness
					if (!materials[shape.mesh.material_ids[0]].metallic_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Metalness)] = fileDirectory + materials[shape.mesh.material_ids[0]].metallic_texname;
						shouldCreateNewMaterialInstance = true;
					}
					else if (!materials[shape.mesh.material_ids[0]].specular_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Metalness)] = fileDirectory + materials[shape.mesh.material_ids[0]].specular_texname;
						shouldCreateNewMaterialInstance = true;
					}

					// Roughness
					if (!materials[shape.mesh.material_ids[0]].roughness_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Roughness)] = fileDirectory + materials[shape.mesh.material_ids[0]].roughness_texname;
						shouldCreateNewMaterialInstance = true;
					}

					// Ambient Occlusion
					if (!materials[shape.mesh.material_ids[0]].ambient_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::AmbientOcclusion)] = fileDirectory + materials[shape.mesh.material_ids[0]].ambient_texname;
						shouldCreateNewMaterialInstance = true;
					}
				}

				Ref<MaterialInstance> materialInstance;
				if (shouldCreateNewMaterialInstance)
				{
					PBRTextureMaps textureMaps = TextureLibrary::LoadTextureMaps(texturePaths);
					materialInstance = MaterialLibrary::CreateInstanceFrom(pbrMaterial);
					materialInstance->SetValue(PBRMaterial::AlbedoMap, textureMaps.Albedo);
					materialInstance->SetValue(PBRMaterial::NormalMap, textureMaps.Normal);
					materialInstance->SetValue(PBRMaterial::MetalnessMap, textureMaps.Metalness);
					materialInstance->SetValue(PBRMaterial::RoughnessMap, textureMaps.Roughness);
					materialInstance->SetValue(PBRMaterial::AmbientOcclusionMap, textureMaps.AmbientOcclusion);
				}
				else
				{
					materialInstance = MaterialLibrary::GetDefaultInstanceFrom(pbrMaterial);
				}

				for (const auto& index : shape.mesh.indices)
				{
					Vertex vertex{};

					vertex.Position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.Normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};

					vertex.TexCoord = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};

					if (uniqueVertices.count(vertex) == 0)
					{
						uniqueVertices[vertex] = static_cast<uint32_t>(submesh.Positions.size());
						submesh.Positions.push_back(vertex.Position);
						submesh.Normals.push_back(vertex.Normal);
						submesh.TexCoords.push_back(vertex.TexCoord);
						submesh.MaterialIndices.push_back(materialInstance->Index);
					}
					submesh.Indices.push_back(uniqueVertices[vertex]);
				}
				model->Submeshes.push_back(BufferManager::AllocateSubmeshFromMeshBuffer(model->_Mesh, submesh));
				model->Materials.push_back(materialInstance);
			}
			return model;
		}
	}

	namespace glTFLoader
	{
		static Ref<Model> Load(const std::string& filePath)
		{
			Ref<Model> model = CreateRef<Model>();
			/**
			 * TODO:
			 */
			return model;
		}
	}

	std::unordered_map<std::string, Ref<Model>> ModelCache;
	constexpr std::array<const char*, static_cast<int>(PrimitiveMesh::PRIMITIVE_MESH_COUNT)> PrimitivePaths = {
		"../Tunti/assets/primitives/cube.obj",
		"../Tunti/assets/primitives/sphere.obj",
		"../Tunti/assets/primitives/cone.obj",
		"../Tunti/assets/primitives/cylinder.obj",
		"../Tunti/assets/primitives/plane.obj"
	};

	Ref<Model> ModelLibrary::Load(const std::string& filePath)
	{
		const auto& modelCacheIt = ModelCache.find(filePath);

		if (modelCacheIt == ModelCache.end())
		{
			std::string fileExtension = std::filesystem::path(filePath).extension().string();
			if (fileExtension == ".obj")
			{
				Ref<Model> model = ObjLoader::Load(filePath);
				return ModelCache.insert(ModelCache.end(), { filePath, model })->second;
			}
			else if (fileExtension == ".gltf")
			{
				Ref<Model> model = glTFLoader::Load(filePath);
				return ModelCache.insert(ModelCache.end(), { filePath, model })->second;
			}

			Log::Error("Tunti Engine only supports OBJ and glTF model formats!");
			return Ref<Model>();
		}

		return modelCacheIt->second;
	}

	Ref<Model> ModelLibrary::LoadPrimitive(PrimitiveMesh primitive)
	{
		return Load(PrimitivePaths[static_cast<int>(primitive)]);
	}

	std::string ModelLibrary::GetModelPath(const Ref<Model>& model)
	{
		for (const auto& [path, modelRef] : ModelCache)
		{
			if (modelRef == model)
				return path;
		}
		return std::string{};
	}

	void ModelLibrary::Flush()
	{
		ModelCache.clear();
	}
}