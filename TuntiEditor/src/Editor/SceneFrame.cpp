#include "SceneFrame.h"

namespace TEditor
{
	SceneFrame::SceneFrame(uint32_t viewportWidth, uint32_t viewportHeight)
		: m_CameraController(60.0f, viewportWidth, viewportHeight)
	{
		std::shared_ptr<Doge::Shader> shader = Doge::ShaderLibrary::CreateShader("assets/shaders/PhongLighting.glsl");

		std::shared_ptr<Doge::Material> material = std::make_shared<Doge::Material>(*shader);
		material->SetBaseColor(glm::vec3(1.0f));
		material->SetBaseShininess(32.0f);

		Doge::Cuboid cube(glm::vec3(1.0f));
		Doge::RenderData cubeData = Doge::RenderDataManager::Construct(cube.GetMesh(), material);
		cubeData.Selected = true;
		m_RenderDatas.push_back(cubeData);
	}

	void SceneFrame::OnUpdate(float dt)
	{
		m_CameraController.OnUpdate(dt);
	}

	void SceneFrame::Render()
	{
		for (const auto& renderData : m_RenderDatas)
		{
			Doge::Renderer::Submit(renderData);
		}
		Doge::Renderer::RenderIndexed(m_CameraController.GetCamera());
	}

	void SceneFrame::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		Doge::Renderer::GetFramebuffer()->Resize(viewportWidth, viewportHeight);
	}
}