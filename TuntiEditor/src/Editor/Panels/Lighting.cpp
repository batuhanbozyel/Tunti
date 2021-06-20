#include "Tunti.h"
#include "Lighting.h"
#include "SceneHierarchy.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "Editor/Utils.h"

namespace TEditor
{
	void Lighting::OnImGuiRender()
	{
		ImGui::Begin("Lighting");

		Tunti::Entity& selectedEntity = SceneHierarchy::GetSelectedEntity();

		Utils::DrawProperty<Tunti::EnvironmentMapTexture>("Environment Map", []()
		{
			static std::string currentEnvironmentMapPath = "Default";
			ImGui::Text(currentEnvironmentMapPath.c_str());

			float contentWidth = ImGui::GetContentRegionAvail().x;
			ImGui::SameLine(contentWidth - contentWidth / 20.0f, 0.0f);
			if (ImGui::Button("...", { 30, 20 }))
			{
				std::string path = Utils::OpenFileDialog();
				if (!path.empty())
				{
					currentEnvironmentMapPath = path;
					auto environmentMap = Tunti::TextureLibrary::LoadEnvironmentMap(currentEnvironmentMapPath);
					Tunti::Renderer::GetRenderPipeline()->SubmitEnvironmentMap(environmentMap);
				}
			}

			Utils::DrawFloatControl("Intensity", Tunti::SceneSettings::Lighting::EnvironmentMapIntensity, 0.5f, glm::vec2{ 0.0f, 5.0f }, 0.01f, "%.2f");
		});

		ImGui::End();
	}
}