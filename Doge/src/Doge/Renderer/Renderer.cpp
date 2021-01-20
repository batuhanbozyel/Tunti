#include "pch.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRenderer.h"

#include "Doge/Core/Window.h"
#include "Doge/Utility/Mesh.h"
#include "Doge/Utility/Camera.h"
#include "Doge/Utility/Material.h"

namespace Doge
{
	decltype(RendererAPI::None) Renderer::s_GraphicsAPI = RendererAPI::None;
	RendererAPI* Renderer::s_Instance = nullptr;

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

	void Renderer::DrawMesh(const GraphicsBuffer& mesh, const Ref<MaterialInstance>& materialInstance, const glm::mat4& transform)
	{
		auto& meshQueue = s_Instance->s_RenderQueue.MeshQueue;
		const Ref<Material>& material = materialInstance->GetParentMaterial();

		((meshQueue[material])[materialInstance]).push_back({ mesh, transform });
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		s_Instance->BeginScene(camera);
	}

	void Renderer::EndScene()
	{
		for (const auto& renderPass : s_Instance->RenderPasses)
			renderPass();

 		s_Instance->EndScene();
		s_Instance->s_RenderQueue.MeshQueue.clear();
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