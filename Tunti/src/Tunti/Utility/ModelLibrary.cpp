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

			auto HashIndices = [](const tinyobj::index_t& val)
			{
				int seed = 0xa5a8ae1e;
				const void* key = &val;
				int len = sizeof(val);

				const uint32_t m = 0x5bd1e995;
				const int32_t r = 24;
				uint32_t h = seed ^ len;
				const uint8_t* data = (const uint8_t*)key;

				while (len >= 4) {
					uint32_t k = *(uint32_t*)data;
					k *= m;
					k ^= k >> r;
					k *= m;
					h *= m;
					h ^= k;
					data += 4;
					len -= 4;
				}

				switch (len) {
					case 3: h ^= data[2] << 16;
					case 2: h ^= data[1] << 8;
					case 1: h ^= data[0];
					h *= m;
				};

				h ^= h >> 13;
				h *= m;
				h ^= h >> 15;

				return h;
			};
			std::unordered_map<tinyobj::index_t, uint32_t, decltype(HashIndices)> uniqueVertices(10, HashIndices);
			for (const tinyobj::shape_t& shape : shapes)
			{
				Submesh submesh;

				std::array<std::string, static_cast<uint32_t>(PBRTextureMap::COUNT)> texturePaths;
				if (materials.size())
				{
					// Albedo
					if (!materials[shape.mesh.material_ids[0]].diffuse_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Albedo)] = fileDirectory + materials[shape.mesh.material_ids[0]].diffuse_texname;
					}

					// Normal
					if (!materials[shape.mesh.material_ids[0]].normal_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Normal)] = fileDirectory + materials[shape.mesh.material_ids[0]].normal_texname;
					}
					else if (!materials[shape.mesh.material_ids[0]].bump_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Normal)] = fileDirectory + materials[shape.mesh.material_ids[0]].bump_texname;
					}

					// Metalness
					if (!materials[shape.mesh.material_ids[0]].metallic_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Metalness)] = fileDirectory + materials[shape.mesh.material_ids[0]].metallic_texname;
					}
					else if (!materials[shape.mesh.material_ids[0]].specular_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Metalness)] = fileDirectory + materials[shape.mesh.material_ids[0]].specular_texname;
					}

					// Roughness
					if (!materials[shape.mesh.material_ids[0]].roughness_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::Roughness)] = fileDirectory + materials[shape.mesh.material_ids[0]].roughness_texname;
					}

					// Ambient Occlusion
					if (!materials[shape.mesh.material_ids[0]].ambient_texname.empty()) {
						texturePaths[static_cast<uint32_t>(PBRTextureMap::AmbientOcclusion)] = fileDirectory + materials[shape.mesh.material_ids[0]].ambient_texname;
					}
				}

				Ref<MaterialInstance> materialInstance;
				PBRTextureMaps textureMaps = TextureLibrary::LoadTextureMaps(texturePaths);
				materialInstance = MaterialLibrary::CreateInstanceFrom(pbrMaterial);
				materialInstance->SetValue(PBRMaterial::AlbedoMap, textureMaps.Albedo);
				materialInstance->SetValue(PBRMaterial::NormalMap, textureMaps.Normal);
				materialInstance->SetValue(PBRMaterial::MetalnessMap, textureMaps.Metalness);
				materialInstance->SetValue(PBRMaterial::RoughnessMap, textureMaps.Roughness);
				materialInstance->SetValue(PBRMaterial::AmbientOcclusionMap, textureMaps.AmbientOcclusion);

				for (const auto& index : shape.mesh.indices)
				{
					if (uniqueVertices.count(index) == 0)
					{
						uniqueVertices[index] = static_cast<uint32_t>(submesh.Positions.size());

						submesh.Positions.push_back({
							attrib.vertices[3 * index.vertex_index + 0],
							attrib.vertices[3 * index.vertex_index + 1],
							attrib.vertices[3 * index.vertex_index + 2]
						});

						submesh.Normals.push_back({attrib.normals[3 * index.normal_index + 0],
							attrib.normals[3 * index.normal_index + 1],
							attrib.normals[3 * index.normal_index + 2]
						});

						submesh.TexCoords.push_back({
							attrib.texcoords[2 * index.texcoord_index + 0],
							attrib.texcoords[2 * index.texcoord_index + 1]
						});

						submesh.MaterialIndices.push_back(materialInstance->Index);
					}
					submesh.Indices.push_back(uniqueVertices[index]);
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