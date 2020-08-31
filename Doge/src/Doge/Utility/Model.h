#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Doge
{
	class Mesh;

	enum class TextureType;

	class Model
	{
	public:
		Model(const std::string& filePath);

		void Translate(const glm::vec3& pos);
		void Rotate(float angle, const glm::vec3& axis = glm::vec3(-1.0f, 0.0f, 0.0f));

		inline const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }
	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

		void AddTexturePath(std::vector<std::pair<std::string, TextureType>>& texturePaths, aiMaterial* material, aiTextureType type, TextureType textureType);
	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;
	};
}