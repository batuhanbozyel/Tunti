#include <glad/glad.h>
#include "Tunti.h"
#include "EditorLayer.h"
#include "Utils.h"

#include <commdlg.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <GLFW/glfw3.h>

#include "Platform/OpenGL/OpenGLEditor.h"

#include "Panels/SceneViewport.h"
#include "Panels/SceneHierarchy.h"
#include "Panels/Inspector.h"
#include "Panels/Lighting.h"

namespace TEditor
{
	static void SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	EditorLayer::EditorLayer()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Medium.ttf", 16.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Medium.ttf", 16.0f);

		SetDarkThemeColors();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		switch (Tunti::Application::GetRenderAPI())
		{
			case Tunti::RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); break;
			case Tunti::RenderAPI::OpenGL:
			{
				m_EditorAPI = Tunti::CreateScope<OpenGLEditor>();
				break;
			}
		}
		LOG_ASSERT(m_EditorAPI, "RendererAPI is not specified!");
	}

	void EditorLayer::OnAttach()
	{
		SceneViewport::OnStart();
	}

	void EditorLayer::OnEvent(Tunti::Event& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		e.Handled |= e.IsInCategory(Tunti::EventCategoryMouse) & io.WantCaptureMouse;
		e.Handled |= e.IsInCategory(Tunti::EventCategoryKeyboard) & io.WantCaptureKeyboard;
		Tunti::EventDispatcher dispatcher(e);

		SceneViewport::OnEvent(e);
	}

	void EditorLayer::OnUpdate(double dt)
	{
		SceneViewport::OnEditorUpdate(dt);

		Begin();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
					Tunti::Application::GetInstance()->Shutdown();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		SceneViewport::OnImGuiRender();
		SceneHierarchy::OnImGuiRender();
		Inspector::OnImGuiRender();
		Lighting::OnImGuiRender();

		ImGui::Begin("Renderer Info");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		{
			// VSync
			bool vsync = Tunti::Application::GetWindow()->IsVSync();
			ImGui::Checkbox("VSync", &vsync);
			Tunti::Application::GetWindow()->SetVSync(vsync);
		}
		ImGui::End();
		
		ImGui::Begin("Shadows");
		{
			Utils::DrawFloatControl("Max Shadow Distance", Tunti::SceneSettings::ShadowMap::MaxShadowDistance, 100.0f, glm::vec2{ 0.0f, 1000.0f }, 1.0f, "%.1f");
			Utils::DrawFloatControl("Cascade NearPlane Offset", Tunti::SceneSettings::ShadowMap::CascadeNearPlaneOffset, -15.0f, glm::vec2{ -100.0f,  Tunti::SceneSettings::ShadowMap::CascadeFarPlaneOffset }, 0.5f, "%.1f");
			Utils::DrawFloatControl("Cascade FarPlane Offset", Tunti::SceneSettings::ShadowMap::CascadeFarPlaneOffset, -15.0f, glm::vec2{ Tunti::SceneSettings::ShadowMap::CascadeNearPlaneOffset, 100.0f }, 0.5f, "%.1f");

			ImGui::Image(reinterpret_cast<void*>((uint64_t)Tunti::Renderer::GetRenderPipeline()->GetDebugOutputTexture()), ImVec2(360, 360), ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();

		End();
	}

	void EditorLayer::Begin()
	{
		m_EditorAPI->BeginRender();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
	}

	void EditorLayer::End()
	{
		ImGui::End();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(
			(float)Tunti::Application::GetWindow()->GetWindowProps().Width,
			(float)Tunti::Application::GetWindow()->GetWindowProps().Height);

		m_EditorAPI->EndRender();
	}
}