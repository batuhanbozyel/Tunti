#include "Tunti.h"
#include "EditorLayer.h"

#include "Platform/OpenGL/OpenGLEditorLayer.h"

namespace TEditor
{
	EditorLayer* EditorLayer::s_EditorAPI = nullptr;

	EditorLayer::EditorLayer()
		: m_Scene(Tunti::Application::GetActiveWindow()->GetWindowProps().Width,
			Tunti::Application::GetActiveWindow()->GetWindowProps().Height)
	{
		
	}

	EditorLayer* EditorLayer::Create()
	{
		switch (Tunti::Renderer::GetAPI())
		{
			case Tunti::RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
			case Tunti::RendererAPI::OpenGL: return s_EditorAPI = new OpenGLEditorLayer;
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
		delete s_EditorAPI;
	}

	void EditorLayer::OnEvent(Tunti::Event& e)
	{
		if (!m_ScenePlay)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(Tunti::EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(Tunti::EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
		Tunti::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Tunti::KeyPressedEvent>(BIND_EVENT_FN(OnKeyPress));
	}

	void EditorLayer::OnUpdate()
	{
		m_Scene.Render();

		s_EditorAPI->ImGuiBeginRender();

		MenuBarView();
		SceneView();
		StatsView();

		s_EditorAPI->ImGuiEndRender();

		if (m_ScenePlay)
		{
			m_Scene.OnUpdate();
		}
	}

	void EditorLayer::StopScenePlay()
	{
		if (m_ScenePlay == true)
		{
			m_ScenePlay = false;
			m_Scene = SceneFrame(Tunti::Application::GetActiveWindow()->GetWindowProps().Width,
								 Tunti::Application::GetActiveWindow()->GetWindowProps().Height);
			Tunti::Application::EnableCursor();

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsLight();

			// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}
		}
	}

	void EditorLayer::StartScenePlay()
	{
		if (m_ScenePlay == false)
		{
			m_ScenePlay = true;
			Tunti::Application::SetCursorPos(Tunti::Application::GetActiveWindow()->GetWindowProps().Width / 2.0f,
											Tunti::Application::GetActiveWindow()->GetWindowProps().Height / 2.0f);
			Tunti::Application::DisableCursor();

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsClassic();

			// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

		}
	}

	void EditorLayer::MenuBarView()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit")) Tunti::Application::GetInstance()->Shutdown();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::SceneView()
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

		//ImGui::Image(reinterpret_cast<void*>(m_Scene.GetSceneTexture()), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
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

	bool EditorLayer::OnKeyPress(Tunti::KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == Tunti::Input::Key::Escape)
		{
			StopScenePlay();
			return true;
		}
		return false;
	}
}