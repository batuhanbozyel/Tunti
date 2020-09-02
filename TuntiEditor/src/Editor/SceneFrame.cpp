#include "SceneFrame.h"

namespace TEditor
{
	SceneFrame::SceneFrame(uint32_t viewportWidth, uint32_t viewportHeight)
		: m_SceneFramebuffer(std::make_unique<Doge::Framebuffer>(Doge::FramebufferSpecification(viewportWidth, viewportHeight)))
	{
		const Doge::Shader* shader = Doge::ShaderLibrary::CreateShader("assets/shaders/PhongLighting.glsl");

		std::shared_ptr<Doge::Material> material = std::make_shared<Doge::Material>(*shader);
		material->SetBaseColor(glm::vec3(1.0f));
		material->SetBaseShininess(32.0f);

		Doge::Model backpack("assets/models/backpack/backpack.obj");
		Doge::Model handgun("assets/models/gun/M1911_01.obj");

		Doge::RenderData backpackData = Doge::RenderDataManager::ConstructBatched(backpack.GetMeshes(), material);
		for (uint32_t i = 0; i < 10; i++)
			for (uint32_t j = 0; j < 10; j++)
			{
				backpackData.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(i * 4.0f, j * 4.0f, 0.0f));
				m_RenderDatas.push_back(backpackData);
			}
	}

	void SceneFrame::OnUpdate(float dt)
	{
		Doge::Renderer::GetCamera()->Rotate(Doge::Input::GetMousePos());

		if (Doge::Input::IsKeyPressed(KEY_W)) Doge::Renderer::GetCamera()->Move(KEY_W, dt);
		if (Doge::Input::IsKeyPressed(KEY_A)) Doge::Renderer::GetCamera()->Move(KEY_A, dt);
		if (Doge::Input::IsKeyPressed(KEY_S)) Doge::Renderer::GetCamera()->Move(KEY_S, dt);
		if (Doge::Input::IsKeyPressed(KEY_D)) Doge::Renderer::GetCamera()->Move(KEY_D, dt);
	}

	void SceneFrame::Render()
	{
		for (const auto& renderData : m_RenderDatas)
		{
			Doge::Renderer::Submit(renderData);
		}

		m_SceneFramebuffer->Bind();
		Doge::Renderer::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Doge::Renderer::Clear();
		Doge::Renderer::DrawIndexed();
		m_SceneFramebuffer->Unbind();
	}

	void SceneFrame::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_SceneFramebuffer->Resize(viewportWidth, viewportHeight);
	}

}
