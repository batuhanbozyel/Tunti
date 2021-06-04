#include "pch.h"
#include "Mesh.h"
#include "Light.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Renderer.h"
#include "BufferManager.h"

#include "Platform/OpenGL/OpenGLRenderer.h"

#include "Tunti/Core/Window.h"

namespace Tunti
{
	void Renderer::Init()
	{
		if (s_Instance)
		{
			Log::Warn("Renderer has been already initialized!");
			return;
		}

		switch (API)
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); break;
			case RendererAPI::OpenGL: s_Instance = new OpenGLRenderer(); break;
		}

		Log::Trace("Renderer has initialized successfully!");
	}

	void Renderer::Shutdown()
	{
		if (s_Instance)
			delete s_Instance;
	}

	void Renderer::Submit(const std::function<void()>& renderPass)
	{
		s_Instance->RenderPasses.push_back(renderPass);
	}

	void Renderer::SubmitDirectionalLight(const _DirectionalLight& directionalLight)
	{
		s_Instance->LightQueue.DirectionalLight = directionalLight;
	}

	void Renderer::SubmitPointLight(const _PointLight& pointLight)
	{
		s_Instance->LightQueue.PointLights[s_Instance->LightQueue.PointLightCount++] = pointLight;
	}

	void Renderer::DrawMesh(const MeshBuffer& mesh, const SubmeshQueueElementList& submeshes, const glm::mat4& transform)
	{
		const auto& [submeshBuffers, materialInstances] = submeshes;
		s_Instance->MeshQueue[materialInstances[0]->BaseMaterial][mesh].push_back({ submeshes, transform });
	}

	void Renderer::ResizeFramebuffers(uint32_t width, uint32_t height)
	{
		s_Instance->ResizeFramebuffers(width, height);
	}

	Texture2D Renderer::GetFramebufferTexture()
	{
		return s_Instance->GetFramebufferTexture();
	}

	void Renderer::DrawScene(const Camera& camera, const glm::mat4& view, const glm::vec3& position)
	{
		s_Instance->BeginScene(camera, view, position);

		for (const auto& renderPass : s_Instance->RenderPasses)
			renderPass();

		s_Instance->EndScene();
		s_Instance->MeshQueue.clear();
		s_Instance->LightQueue.DirectionalLight = _DirectionalLight();
		s_Instance->LightQueue.PointLightCount = 0;
	}

	void Renderer::SubmitEnvironmentMap(EnvironmentMapTexture environmentMap)
	{
		s_Instance->SetEnvironmentMap(environmentMap);
	}

}