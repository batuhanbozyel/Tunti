#include "Tunti.h"
#include "OpenGLEditor.h"

namespace TEditor
{
	OpenGLEditor::OpenGLEditor()
	{
		ImGui_ImplGlfw_InitForOpenGL(Tunti::Application::GetWindow()->GetHandle(), true);
		ImGui_ImplOpenGL3_Init("#version 450");

		BeginRender = []()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		};

		EndRender = []()
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
		};
	}

	OpenGLEditor::~OpenGLEditor()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}