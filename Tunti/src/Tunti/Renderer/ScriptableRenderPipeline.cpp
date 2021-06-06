#include "pch.h"
#include "ScriptableRenderPipeline.h"

namespace Tunti
{
	ScriptableRenderPipeline::ScriptableRenderPipeline()
		: EnvironmentMap(TextureLibrary::DefaultEnvironmentMap())
	{

	}

	void ScriptableRenderPipeline::SubmitPointLight(const _PointLight& pointLight)
	{
		LightQueue.PointLights[LightQueue.PointLightCount++] = pointLight;
	}

	void ScriptableRenderPipeline::SubmitDirectionalLight(const _DirectionalLight& directionalLight)
	{
		LightQueue.DirectionalLight = directionalLight;
	}

	void ScriptableRenderPipeline::SubmitEnvironmentMap(EnvironmentMapTexture environmentMap)
	{
		EnvironmentMap = environmentMap;
	}

	void ScriptableRenderPipeline::SubmitMesh(const MeshBuffer& mesh, const SubmeshQueueElementList& submeshes, const glm::mat4& transform)
	{
		const auto& [submeshBuffers, materialInstances] = submeshes;
		MeshQueue[materialInstances[0]->BaseMaterial][mesh].push_back({ submeshes, transform });
	}

	Texture2D ScriptableRenderPipeline::Execute(const ShaderCameraContainer& shaderCamera)
	{
		Texture2D outputTexture = ExecuteImpl(shaderCamera);

		MeshQueue.clear();
		LightQueue.DirectionalLight = _DirectionalLight();
		LightQueue.PointLightCount = 0;

		return outputTexture;
	}
}