#include "Doge.h"
#include "EditorLayer.h"

namespace TEditor
{
	EditorLayer::EditorLayer()
		: m_Scene(Doge::Application::GetActiveWindow()->GetWindowProps().Width,
				  Doge::Application::GetActiveWindow()->GetWindowProps().Height)
	{
		
	}

	void EditorLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer bindings
		GLFWwindow* window = Doge::Application::GetInstance()->GetActiveWindow()->GetNativeWindow();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	void EditorLayer::OnDetach()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
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

		ImGuiBeginRender();

		MenuBarView();
		SceneView(dt);
		StatsView();

		ImGuiEndRender();

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
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
	}

	void EditorLayer::ImGuiEndRender()
	{
		ImGui::End();

		const Doge::Application* app = Doge::Application::GetInstance();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)app->GetActiveWindow()->GetWindowProps().Width, (float)app->GetActiveWindow()->GetWindowProps().Height);

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
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