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
		auto& tag = entity.GetComponent<Tunti::TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((s_Data.SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			s_Data.SelectedEntity = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			SceneViewport::GetSceneContext().DestroyEntity(entity);
			if (s_Data.SelectedEntity == entity)
				s_Data.SelectedEntity = {};
		}
	}

	// SceneHierarchyPanel

	void SceneHierarchy::OnRender()
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

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				context.CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	Tunti::Entity& SceneHierarchy::GetSelectedEntity()
	{
		return s_Data.SelectedEntity;
	}

}