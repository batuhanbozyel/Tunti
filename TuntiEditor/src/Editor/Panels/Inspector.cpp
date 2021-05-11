#include "Tunti.h"
#include "Inspector.h"
#include "SceneHierarchy.h"

#include "Editor/Utils.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace TEditor
{
	static constexpr std::array<const char*, 4> LightTypeNames = {{
		{"Directional Light"},
		{"Point Light"},
		{"Spot Light"},
		{"Area Light"}
	}};

	void Inspector::OnImGuiRender()
	{
		ImGui::Begin("Inspector");
		Tunti::Entity& selectedEntity = SceneHierarchy::GetSelectedEntity();
		if (selectedEntity)
		{
			if (selectedEntity.HasComponent<Tunti::TagComponent>())
			{
				auto& tag = selectedEntity.GetComponent<Tunti::TagComponent>().Tag;

				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				std::strncpy(buffer, tag.c_str(), sizeof(buffer));
				if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				{
					tag = std::string(buffer);
				}
			}

			ImGui::SameLine();
			ImGui::PushItemWidth(-1);

			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					if (!selectedEntity.HasComponent<Tunti::CameraComponent>())
						selectedEntity.AddComponent<Tunti::CameraComponent>();
					else
						Tunti::Log::Warn("This entity already has the Camera Component!");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Mesh Renderer"))
				{
					if (!selectedEntity.HasComponent<Tunti::MeshRendererComponent>())
						selectedEntity.AddComponent<Tunti::MeshRendererComponent>();
					else
						Tunti::Log::Warn("This entity already has the Mesh Renderer Component!");

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::PopItemWidth();

			Utils::DrawComponent<Tunti::TransformComponent>("Transform", selectedEntity, [](auto& component)
			{
				Utils::DrawVec3Control("Position", component.Position);
				Utils::DrawVec3Control("Rotation", component.Rotation);
				Utils::DrawVec3Control("Scale", component.Scale, 1.0f);
			});

			Utils::DrawComponent<Tunti::CameraComponent>("Camera", selectedEntity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);
			});

			Utils::DrawComponent<Tunti::MeshRendererComponent>("Mesh Renderer", selectedEntity, [](auto& component)
			{
				ImGui::Text(component.ModelPath.c_str());

				float contentWidth = ImGui::GetContentRegionAvail().x;
				ImGui::SameLine(contentWidth - contentWidth / 20.0f, 0.0f);
				if (ImGui::Button("...", { 30, 20 }))
				{
					std::string filePath = Utils::OpenFileDialog();
				}
			});

			Utils::DrawComponent<Tunti::LightComponent>("Light", selectedEntity, [](auto& component)
			{
				ImGui::PushID("Type");
				Utils::SetLabel("Type");
				if (ImGui::BeginCombo("Type", LightTypeNames[static_cast<int>(component._Light.Type)]))
				{
					for (int32_t i = 0; i < LightTypeNames.size(); i++) {
						if (ImGui::Selectable(LightTypeNames[i])) {
							component._Light.Type = static_cast<Tunti::LightType>(i);
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopID();

				Utils::DrawFloatControl("Intensity", component._Light.Intensity, 1.0f, { 0.0f, 10.0f }, 0.05f);
				Utils::DrawFloatControl("Constant", component._Light.Constant, 1.0f, { 0.0f, 10.0f }, 0.05f);
				Utils::DrawFloatControl("Linear", component._Light.Linear, 0.09f, { 0.0f, 1.0f }, 0.001f);
				Utils::DrawFloatControl("Quadratic", component._Light.Quadratic, 0.032f, { 0.0f, 1.0f }, 0.001f, "%.3f");
			});
		}

		ImGui::End();
	}
}
