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
			std::unordered_map<Vertex, uint32_t> uniqueVertices{};
			for (const auto& shape : shapes)
			{
				Mesh mesh;
				mesh.Name = shape.name;
				std::array<std::string, static_cast<uint32_t>(TextureType::COUNT)> texturePaths;
				if (materials.size())
				{
					// Albedo
					if (!materials[shape.mesh.material_ids[0]].diffuse_texname.empty()) {
						texturePaths[static_cast<uint32_t>(TextureType::Albedo)] = fileDirectory + materials[shape.mesh.material_ids[0]].diffuse_texname;
					}

					// Normal
					if (!materials[shape.mesh.material_ids[0]].normal_texname.empty()) {
						texturePaths[static_cast<uint32_t>(TextureType::Normal)] = fileDirectory + materials[shape.mesh.material_ids[0]].normal_texname;
					}
					else if (!materials[shape.mesh.material_ids[0]].bump_texname.empty()) {
						texturePaths[static_cast<uint32_t>(TextureType::Normal)] = fileDirectory + materials[shape.mesh.material_ids[0]].bump_texname;
					}

					// Metalness
					if (!materials[shape.mesh.material_ids[0]].metallic_texname.empty()) {
						texturePaths[static_cast<uint32_t>(TextureType::Metalness)] = fileDirectory + materials[shape.mesh.material_ids[0]].metallic_texname;
					}
					else if (!materials[shape.mesh.material_ids[0]].specular_texname.empty()) {
						texturePaths[static_cast<uint32_t>(TextureType::Metalness)] = fileDirectory + materials[shape.mesh.material_ids[0]].specular_texname;
					}

					// Roughness
					if (!materials[shape.mesh.material_ids[0]].roughness_texname.empty()) {
						texturePaths[static_cast<uint32_t>(TextureType::Roughness)] = fileDirectory + materials[shape.mesh.material_ids[0]].roughness_texname;
					}

					// Ambient Occlusion
					if (!materials[shape.mesh.material_ids[0]].ambient_texname.empty()) {
						texturePaths[static_cast<uint32_t>(TextureType::AmbientOcclusion)] = fileDirectory + materials[shape.mesh.material_ids[0]].ambient_texname;
					}
				}
				Ref<TextureMap> textureMap = TextureLibrary::LoadTextureMap(texturePaths);

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
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};

					if (uniqueVertices.count(vertex) == 0)
					{
						uniqueVertices[vertex] = static_cast<uint32_t>(mesh.Position.size());
						mesh.Position.push_back(vertex.Position);
						mesh.Normal.push_back(vertex.Normal);
						mesh.TexCoord.push_back(vertex.TexCoord);
						mesh.TexIndex.push_back(textureMap->Index);
					}
					mesh.Indices.push_back(uniqueVertices[vertex]);
				}
				model->Meshes.emplace_back(std::move(mesh));
				model->TextureMaps.emplace_back(std::move(textureMap));
				model->MaterialInstances.emplace_back(Material::DefaulMaterialInstance());
			}
			return model;
		}
	}

	namespace glTFLoader
	{
		static Ref<Model> Load(const std::string& filePath)
		{
			Ref<Model> model = CreateRef<Model>();

			return model;
		}
	}

	namespace ModelLibrary
	{
		static std::unordered_map<std::string, Ref<Model>> s_ModelCache;
		constexpr std::array<const char*, static_cast<int>(PrimitiveMesh::PRIMITIVE_MESH_COUNT)> PrimitivePaths = {
			"../Tunti/assets/primitives/cube.obj",
			"../Tunti/assets/primitives/sphere.obj",
			"../Tunti/assets/primitives/cone.obj",
			"../Tunti/assets/primitives/cylinder.obj",
			"../Tunti/assets/primitives/plane.obj"
		};

		Ref<Model> Load(const std::string& filePath)
		{
			const auto& modelCacheIt = s_ModelCache.find(filePath);

			if (modelCacheIt == s_ModelCache.end())
			{
				std::string fileExtension = std::filesystem::path(filePath).extension().string();
				if (fileExtension == ".obj")
				{
					Ref<Model> model = ObjLoader::Load(filePath);

					return s_ModelCache.insert(s_ModelCache.end(), { filePath, model })->second;
				}
				else if (fileExtension == ".gltf")
				{
					Ref<Model> model = glTFLoader::Load(filePath);

					return s_ModelCache.insert(s_ModelCache.end(), { filePath, model })->second;
				}

				Log::Error("Tunti Engine only supports OBJ and glTF model formats!");
				return Ref<Model>();
			}

			return modelCacheIt->second;
		}

		Ref<Model> LoadPrimitive(PrimitiveMesh primitive)
		{
			return Load(PrimitivePaths[static_cast<int>(primitive)]);
		}

		std::string GetModelPath(const Ref<Model>& model)
		{
			for (const auto& [path, modelRef] : s_ModelCache)
			{
				if (modelRef == model)
					return path;
			}

			return std::string{};
		}

		void Flush()
		{
			s_ModelCache.clear();
		}
	}
}