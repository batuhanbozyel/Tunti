#include "Tunti.h"
#include <imgui.h>
#include "SceneViewport.h"

namespace TEditor
{
	struct SceneData
	{
		Tunti::Scene SceneContext;
		Tunti::Texture2D LastFrame;
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

	}

	void SceneViewport::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene Viewport");
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		ImGui::Image(reinterpret_cast<void*>((uint64_t)s_Data.LastFrame), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
		Tunti::Renderer::OnWindowResize(viewportSize.x, viewportSize.y);
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
		
		s_Data.LastFrame = s_Data.SceneContext.OnUpdateEditor(dt, s_Data.SceneEditorCamera);
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