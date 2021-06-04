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
				float metalness = *(float*)(component.Materials[0]->Properties[(uint32_t)Tunti::PBRMaterial::Metalness].GetPropertyValue());
				float roughness = *(float*)(component.Materials[0]->Properties[(uint32_t)Tunti::PBRMaterial::Roughness].GetPropertyValue());

				Utils::DrawFloatControl("Metalness", metalness, 1.0f, { 0.0f, 1.0f }, 0.005f);
				Utils::DrawFloatControl("Roughness", roughness, 1.0f, { 0.0f, 1.0f }, 0.005f);

				component.Materials[0]->SetValue((uint32_t)Tunti::PBRMaterial::Metalness, metalness);
				component.Materials[0]->SetValue((uint32_t)Tunti::PBRMaterial::Roughness, roughness);
			});

			Utils::DrawComponent<Tunti::LightComponent>("Light", selectedEntity, [](auto& component)
			{
				ImGui::PushID("Type");
				Utils::SetLabel("Type");
				if (ImGui::BeginCombo("Type", LightTypeNames[static_cast<int>(component.Type)]))
				{
					for (int32_t i = 0; i < LightTypeNames.size(); i++)
					{
						if (ImGui::Selectable(LightTypeNames[i]))
						{
							component.Type = static_cast<Tunti::LightType>(i);
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopID();

				if (component.Type == Tunti::LightType::DirectionalLight)
				{
					Utils::DrawFloatControl("Intensity", component.Intensity, 1.0f, { 0.0f, 10.0f }, 0.05f);
				}
				else if (component.Type == Tunti::LightType::PointLight)
				{
					Utils::DrawFloatControl("Constant", component.Constant, 1.0f, { 0.0f, 10.0f }, 0.05f);
					Utils::DrawFloatControl("Linear", component.Linear, 0.09f, { 0.0f, 1.0f }, 0.01f);
					Utils::DrawFloatControl("Quadratic", component.Quadratic, 0.032f, { 0.0f, 1.0f }, 0.01f);
				}
			});
		}

		ImGui::End();
	}
}
