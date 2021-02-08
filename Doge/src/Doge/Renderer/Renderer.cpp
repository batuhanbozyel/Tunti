#include "pch.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "BufferManager.h"

#include "Platform/OpenGL/OpenGLRenderer.h"

#include "Doge/Core/Window.h"
#include "Doge/Utility/Mesh.h"
#include "Doge/Utility/Light.h"
#include "Doge/Utility/Camera.h"
#include "Doge/Utility/Material.h"

namespace Doge
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

	void Renderer::DrawLight(const LightData& light, const glm::mat4& transform)
	{

	}

	void Renderer::DrawMesh(const MeshData& mesh, const Ref<MaterialInstance>& materialInstance, const glm::mat4& transform)
	{
		auto& meshQueue = s_Instance->MeshQueue;
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
		s_Instance->MeshQueue.clear();
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