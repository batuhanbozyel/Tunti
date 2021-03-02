#include "Tunti.h"
#include <imgui.h>
#include "SceneViewport.h"

namespace TEditor
{
	struct SceneData
	{
		Tunti::Scene SceneContext;
		Tunti::EditorCamera SceneEditorCamera{ 90.0f, 16.0f / 9.0f, 0.1f, 10000.0f };
		Tunti::FirstPersonCameraController FPSCamera;

		bool IsSceneHovered = false;

	} static s_Data;

	void SceneViewport::OnEvent(Tunti::Event& e)
	{
		if (s_Data.IsSceneHovered)
		{
			s_Data.SceneEditorCamera.OnEvent(e);
		}
	}

	void SceneViewport::OnStart()
	{
#if 0
		s_Data.FPSCamera.OnStart(s_Data.SceneContext);
		Tunti::Ref<Tunti::Model> cubeModel = Tunti::ModelLibrary::Load("./assets/primitives/cube.obj");
		for (uint32_t i = 0; i < cubeModel->Meshes.size(); i++)
		{
			Tunti::Entity cubeSubentity = s_Data.SceneContext.CreateEntity("Cube_" + std::to_string(i));
			cubeSubentity.AddComponent<Tunti::MeshRendererComponent>(cubeModel->Meshes[i], cubeModel->MaterialInstances[i]).ModelRef = cubeModel;
			cubeSubentity.GetComponent<Tunti::TransformComponent>().Position = glm::vec3(0.0f, 0.0f, -4.0f);
			cubeSubentity.GetComponent<Tunti::TransformComponent>().Rotation = glm::vec3(0.0f, 30.0f, 0.0f);
		}

		Tunti::Ref<Tunti::Model> lightConeModel = Tunti::ModelLibrary::Load("../TuntiEditor/assets/primitives/cone.obj");
		for (uint32_t i = 0; i < lightConeModel->Meshes.size(); i++)
		{
			Tunti::Entity lightConeEntity = s_Data.SceneContext.CreateEntity("LightCone");
			lightConeEntity.AddComponent<Tunti::MeshRendererComponent>(lightConeModel->Meshes[i], lightConeModel->MaterialInstances[i]);
	}
#else
		Tunti::Ref<Tunti::Model> sponzaModel = Tunti::ModelLibrary::Load("../Sandbox/assets/models/sponza/sponza.obj");
		for (uint32_t i = 0; i < sponzaModel->Meshes.size(); i++)
		{
			Tunti::Entity sponzaSubEntity = s_Data.SceneContext.CreateEntity("Sponza_" + std::to_string(i));
			sponzaSubEntity.AddComponent<Tunti::MeshRendererComponent>(sponzaModel->Meshes[i], sponzaModel->MaterialInstances[i]);
		}
#endif
	}

	void SceneViewport::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene Viewport");
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		uint64_t sceneFrame = Tunti::Renderer::GetFramebufferTexture();
		ImGui::Image(reinterpret_cast<void*>(sceneFrame), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
		Tunti::Renderer::ResizeFramebuffers(viewportSize.x, viewportSize.y);
		s_Data.SceneEditorCamera.SetViewportSize(viewportSize.x, viewportSize.y);

		s_Data.IsSceneHovered = ImGui::IsItemHovered();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void SceneViewport::OnEditorUpdate(double dt)
	{
		Tunti::Application::EnableCursor();

		if (s_Data.IsSceneHovered)
			s_Data.SceneEditorCamera.OnUpdate(dt);
		
		s_Data.SceneContext.OnUpdateEditor(dt, s_Data.SceneEditorCamera);
	}

	void SceneViewport::OnPlay(double dt)
	{
		s_Data.FPSCamera.OnUpdate(dt);
		s_Data.SceneContext.OnUpdateRuntime(dt);
	}

	Tunti::Scene& SceneViewport::GetSceneContext()
	{
		return s_Data.SceneContext;
	}

	std::pair<float, float> SceneViewport::GetViewportSize()
	{
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		return { viewportSize.x, viewportSize.y };
	}
}