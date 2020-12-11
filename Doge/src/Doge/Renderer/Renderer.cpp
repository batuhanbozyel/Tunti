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
	Renderer* Renderer::s_Instance = nullptr;

	void Renderer::Init(const WindowProps& props)
	{
		if (s_Instance)
		{
			Log::Warn("Renderer has been already initialized!");
			return;
		}

		switch (s_RendererAPI)
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); break;
			case RendererAPI::OpenGL: s_Instance = new OpenGLRenderer(props); break;
		}

		Log::Info("Renderer has initialized successfully!");
	}

	void Renderer::Submit(const Mesh& mesh, const Ref<MaterialInstance>& material, const glm::mat4& transform, bool isSelected)
	{
		
	}

	void Renderer::Submit(const std::vector<Mesh>& meshes, const Ref<MaterialInstance>& material, const glm::mat4& transform, bool isSelected)
	{
		Submit(Mesh::BatchMeshes(meshes), material, transform, isSelected);
	}

	void Renderer::SetSkybox(CubemapTexture skybox)
	{
		s_Instance->SetSkyboxImpl(skybox);
	}

	void Renderer::ClearSkybox()
	{
		s_Instance->ClearSkyboxImpl();
	}

	void Renderer::FlushRenderer()
	{
		s_Instance->FlushImpl();
	}

	void Renderer::RenderIndexed(const Camera& camera)
	{
		// Render in between Begin and End calls
		s_Instance->BeginRender(camera);
		{
			// Render objects normally
			s_Instance->RenderObjectsIndexed();

			// Render objects in single color with vertices expanded along their normal
			s_Instance->RenderOutlinedObjectsIndexed();

			// Render Light objects in white color
			// Make sure Lights are rendered right after Object Outlining
			// So, We don't have to bind the same shader again
			s_Instance->RenderLightObjectsIndexed();

			// Render Skybox last for optimization
			s_Instance->RenderSkybox();
		}
		s_Instance->EndRender();
	}

	void Renderer::OnWindowResize(WindowResizeEvent& e)
	{
		s_Instance->ResizeFramebuffer(e.GetWidth(), e.GetHeight());
	}
}