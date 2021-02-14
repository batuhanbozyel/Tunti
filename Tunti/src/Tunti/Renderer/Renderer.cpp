#include "pch.h"
#include "Mesh.h"
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
	void Renderer::Init(decltype(RendererAPI::None) api)
	{
		if (s_Instance)
		{
			Log::Warn("Renderer has been already initialized!");
			return;
		}

		s_GraphicsAPI = api;
		switch (api)
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); break;
			case RendererAPI::OpenGL: s_Instance = new OpenGLRenderer(); break;
		}

		Log::Trace("Renderer has initialized successfully!");
	}

	void Renderer::Shutdown()
	{
		if(s_Instance)
			delete s_Instance;
	}

	void Renderer::Submit(const std::function<void()>& renderPass)
	{
		s_Instance->RenderPasses.push_back(renderPass);
	}

	void Renderer::SubmitLight(const Light& light, const glm::vec3& position, const glm::vec4& direction)
	{
		s_Instance->LightQueue.Lights[s_Instance->LightQueue.LightCount++] = LightData(light, position, direction);
	}

	void Renderer::DrawMesh(const MeshData& mesh, const Ref<MaterialInstance>& materialInstance, const glm::mat4& transform)
	{
		auto& meshQueue = s_Instance->MeshQueue;
		const Ref<Material>& material = materialInstance->GetParentMaterial();

		((meshQueue[material])[materialInstance]).push_back({ mesh, transform });
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform, const glm::vec3& position)
	{
		s_Instance->BeginScene(camera, transform, position);
	}

	void Renderer::EndScene()
	{
		for (const auto& renderPass : s_Instance->RenderPasses)
			renderPass();

 		s_Instance->EndScene();
		s_Instance->MeshQueue.clear();
		s_Instance->LightQueue.LightCount = 0;
	}

	void Renderer::SetSkybox(CubemapTexture skybox)
	{
		s_Instance->SetSkybox(skybox);
	}

	void Renderer::ClearSkybox()
	{
		s_Instance->ClearSkybox();
	}
}