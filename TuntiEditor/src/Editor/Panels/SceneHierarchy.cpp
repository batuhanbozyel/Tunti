#include "Tunti.h"

#include "Platform/OpenGL/OpenGLEditor.h"
#include <imgui_internal.h>

#include "SceneHierarchy.h"
#include "SceneViewport.h"

namespace TEditor
{
	struct SceneHierarchyData
	{
		Tunti::Entity SelectedEntity;
	} static s_Data;

	// Utility

	static void DrawEntity(Tunti::Entity& entity)
	{
		auto& tagComponent = entity.GetComponent<Tunti::TagComponent>();

		ImGuiTreeNodeFlags flags = ((s_Data.SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		if (tagComponent.Renaming)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			const ImVec2 lineStart = ImGui::GetCursorScreenPos();
			const ImGuiStyle& style = ImGui::GetStyle();
			float fullWidth = ImGui::GetContentRegionAvail().x;
			float itemWidth = fullWidth * 0.6f;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tagComponent.Tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tagComponent.Tag = std::string(buffer);

			if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
				tagComponent.Renaming = false;
		}

		else
		{
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagComponent.Tag.c_str());
			if (ImGui::IsItemClicked())
				s_Data.SelectedEntity = entity;

			bool entityDeleted = false;
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Rename"))
					tagComponent.Renaming = true;

				if (ImGui::MenuItem("Delete Entity"))
					entityDeleted = true;

				ImGui::EndPopup();
			}

			if (opened)
			{
				ImGui::TreePop();
			}

			if (entityDeleted)
			{
				SceneViewport::GetSceneContext().DestroyEntity(entity);
				if (s_Data.SelectedEntity == entity)
					s_Data.SelectedEntity = {};
			}
		}
	}

	// SceneHierarchyPanel

	void SceneHierarchy::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		Tunti::Scene& context = SceneViewport::GetSceneContext();

		context.GetSceneRegistry().each([&](auto entityID)
		{
			Tunti::Entity entity{ entityID , &context };
			DrawEntity(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			s_Data.SelectedEntity = {};

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				context.CreateEntity("Empty Entity");

			if (ImGui::MenuItem("Cube"))
			{
				Tunti::Ref<Tunti::Model> model = Tunti::ModelLibrary::LoadPrimitive(Tunti::PrimitiveMesh::CUBE);
				Tunti::Entity modelEntity = context.CreateEntity("Cube Entity");
				modelEntity.AddComponent<Tunti::MeshRendererComponent>(model);
			}

			if (ImGui::MenuItem("Sphere"))
			{
				Tunti::Ref<Tunti::Model> model = Tunti::ModelLibrary::LoadPrimitive(Tunti::PrimitiveMesh::SPHERE);
				Tunti::Entity modelEntity = context.CreateEntity("Sphere Entity");
				modelEntity.AddComponent<Tunti::MeshRendererComponent>(model);
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				Tunti::Ref<Tunti::Model> model = Tunti::ModelLibrary::LoadPrimitive(Tunti::PrimitiveMesh::CYLINDER);
				Tunti::Entity modelEntity = context.CreateEntity("Cylinder Entity");
				modelEntity.AddComponent<Tunti::MeshRendererComponent>(model);
			}

			if (ImGui::MenuItem("Cone"))
			{
				Tunti::Ref<Tunti::Model> model = Tunti::ModelLibrary::LoadPrimitive(Tunti::PrimitiveMesh::CONE);
				Tunti::Entity modelEntity = context.CreateEntity("Cone Entity");
				modelEntity.AddComponent<Tunti::MeshRendererComponent>(model);
			}

			if (ImGui::MenuItem("Plane"))
			{
				Tunti::Ref<Tunti::Model> model = Tunti::ModelLibrary::LoadPrimitive(Tunti::PrimitiveMesh::PLANE);
				Tunti::Entity modelEntity = context.CreateEntity("Plane Entity");
				modelEntity.AddComponent<Tunti::MeshRendererComponent>(model);
			}

			if (ImGui::MenuItem("Directional Light"))
			{
				context.CreateEntity("Directional Light").AddComponent<Tunti::LightComponent>();
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	Tunti::Entity& SceneHierarchy::GetSelectedEntity()
	{
		return s_Data.SelectedEntity;
	}
}