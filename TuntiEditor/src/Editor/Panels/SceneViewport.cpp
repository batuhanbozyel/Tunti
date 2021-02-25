#include "Tunti.h"
#include "SceneViewport.h"

#include <imgui.h>

namespace TEditor
{
	struct SceneData
	{
		Tunti::Scene SceneContext;
		Tunti::Entity SceneEditorCamera; // Temporarily an Entity of SceneCamera Component till Editor Camera
	} static s_Data;

	void SceneViewport::OnStart()
	{
		s_Data.SceneEditorCamera = s_Data.SceneContext.CreateEntity("Editor Camera");
		s_Data.SceneEditorCamera.AddComponent<Tunti::CameraComponent>(
			Tunti::Application::GetWindow()->GetWindowProps().Width,
			Tunti::Application::GetWindow()->GetWindowProps().Height);

		Tunti::Ref<Tunti::Model> cubeModel = Tunti::ModelLibrary::Load("./assets/primitives/cube.obj");
		for (uint32_t i = 0; i < cubeModel->Meshes.size(); i++)
		{
			Tunti::Entity cubeSubentity = s_Data.SceneContext.CreateEntity("Cube_" + std::to_string(i));
			cubeSubentity.AddComponent<Tunti::MeshRendererComponent>(cubeModel->Meshes[i], cubeModel->MaterialInstances[i]);
			cubeSubentity.GetComponent<Tunti::TransformComponent>().SetPosition(glm::vec3(0.0f, 0.0f, -4.0f));
			cubeSubentity.GetComponent<Tunti::TransformComponent>().SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));
		}
	}

	void SceneViewport::OnRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene Viewport");
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		uint64_t sceneFrame = Tunti::Renderer::GetFramebufferTexture();
		ImGui::Image(reinterpret_cast<void*>(sceneFrame), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
		Tunti::Renderer::ResizeFramebuffers(viewportSize.x, viewportSize.y);

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void SceneViewport::OnPlay()
	{
		s_Data.SceneContext.OnUpdate();
	}

	Tunti::Scene& SceneViewport::GetSceneContext()
	{
		return s_Data.SceneContext;
	}
}