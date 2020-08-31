#include "Doge.h"
#include "Doge/Core/EntryPoint.h"

class SandboxApp : public Doge::Application
{
public:
	SandboxApp()
		: Application("SandboxApp")
	{
		const Doge::Shader* shader = Doge::ShaderLibrary::CreateShader("assets/shaders/PhongLighting.glsl");

		std::shared_ptr<Doge::Material> material = std::make_shared<Doge::Material>(*shader);
		material->SetBaseColor(glm::vec3(1.0f));
		material->SetBaseShininess(32.0f);

		Doge::Model backpack("assets/models/backpack/backpack.obj");
		Doge::Model handgun("assets/models/gun/M1911_01.obj");

		Doge::RenderData backpackData = Doge::RenderDataManager::ConstructBatched(backpack.GetMeshes(), material, glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));
		for (uint32_t i = 0; i < 10; i++)
			for (uint32_t j = 0; j < 10; j++)
			{
				backpackData.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(i*4.0f, j*4.0f, 0.0f));
				m_RenderDatas.push_back(backpackData);
			}

		DisableCursor();
	}

	virtual void OnUpdate(float dt) override
	{
		Doge::Renderer::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Doge::Renderer::Clear();

		Doge::Renderer::GetCamera()->Rotate(Doge::Input::GetMousePos());

		if (Doge::Input::IsKeyPressed(KEY_ESCAPE)) Shutdown();

		if (Doge::Input::IsMouseButtonPressed(MOUSE_BUTTON_2))
		{
			if (isMouseVisible) DisableCursor();
			else EnableCursor();
		}

		if (Doge::Input::IsKeyPressed(KEY_W)) Doge::Renderer::GetCamera()->Move(KEY_W, dt);
		if (Doge::Input::IsKeyPressed(KEY_A)) Doge::Renderer::GetCamera()->Move(KEY_A, dt);
		if (Doge::Input::IsKeyPressed(KEY_S)) Doge::Renderer::GetCamera()->Move(KEY_S, dt);
		if (Doge::Input::IsKeyPressed(KEY_D)) Doge::Renderer::GetCamera()->Move(KEY_D, dt);

		for (const auto& renderData : m_RenderDatas)
		{
			Doge::Renderer::Submit(renderData);
		}

		Doge::Renderer::DrawIndexed();
	}
private:
	std::vector<Doge::RenderData> m_RenderDatas;
	bool isMouseVisible = false;
};

Doge::Application* CreateApplication()
{
	return new SandboxApp();
}