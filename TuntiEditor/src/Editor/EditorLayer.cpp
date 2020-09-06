#include "Doge.h"
#include "EditorLayer.h"

#include "Platform/OpenGL/OpenGLEditorLayer.h"

namespace TEditor
{
	EditorLayer* EditorLayer::s_EditorAPI = nullptr;

	EditorLayer::EditorLayer()
		: m_Scene(Doge::Application::GetActiveWindow()->GetWindowProps().Width,
			Doge::Application::GetActiveWindow()->GetWindowProps().Height)
	{
		
	}

	EditorLayer* EditorLayer::Create()
	{
		switch (Doge::Renderer::GetAPI())
		{
		case Doge::RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
		case Doge::RendererAPI::OpenGL: return s_EditorAPI = new OpenGLEditorLayer; return s_EditorAPI;
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	void EditorLayer::OnAttach()
	{
		s_EditorAPI->OnAttach();
	}

	void EditorLayer::OnDetach()
	{
		s_EditorAPI->OnDetach();
	}

	void EditorLayer::OnEvent(Doge::Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(Doge::EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(Doge::EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
		Doge::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Doge::KeyPressedEvent>(BIND_EVENT_FN(OnKeyPress));
	}

	void EditorLayer::OnUpdate(float dt)
	{
		m_Scene.Render();

		s_EditorAPI->ImGuiBeginRender();

		MenuBarView();
		SceneView(dt);
		StatsView();

		s_EditorAPI->ImGuiEndRender();

		if (m_ScenePlay)
		{
			m_Scene.OnUpdate(dt);
		}
	}

	void EditorLayer::StopScenePlay()
	{
		m_BlockEvents = true;
		m_ScenePlay = false;
		Doge::Application::EnableCursor();
	}

	void EditorLayer::StartScenePlay()
	{
		m_BlockEvents = false;
		m_ScenePlay = true;
		Doge::Application::DisableCursor();
	}

	void EditorLayer::MenuBarView()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit")) Doge::Application::GetInstance()->Shutdown();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::SceneView(float dt)
	{
		static bool dockspaceOpen = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene", &dockspaceOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		if (ImGui::Button(">"))
		{
			ImVec2 buttonSize = ImGui::GetContentRegionAvail();
			StartScenePlay();
		}
		m_Scene.Resize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));

		ImGui::Image(reinterpret_cast<void*>(m_Scene.GetSceneTexture()), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::StatsView()
	{
		ImGui::Begin("Renderer Stats");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SameLine();
		ImGui::End();
	}

	void EditorLayer::ImGuiBeginRender()
	{
		s_EditorAPI->ImGuiBeginRender();
	}

	void EditorLayer::ImGuiEndRender()
	{
		s_EditorAPI->ImGuiEndRender();
	}

	bool EditorLayer::OnKeyPress(Doge::KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == Doge::Key::Escape)
		{
			StopScenePlay();
			return true;
		}
		return false;
	}
}