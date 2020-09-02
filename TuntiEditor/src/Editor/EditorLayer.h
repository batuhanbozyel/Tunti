#pragma once
#include "Doge/Core/Layer.h"
#include "SceneFrame.h"

#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

namespace TEditor
{
	class EditorLayer : public Doge::Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Doge::Event& e) override;
		virtual void OnUpdate(float dt) override;

		void StopScenePlay();
		void StartScenePlay();
	private:
		void MenuBarView();
		void StatsView();
		void SceneView(float dt);

		void ImGuiBeginRender();
		void ImGuiEndRender();

		bool OnKeyPress(Doge::KeyPressedEvent& e);
	private:
		bool m_BlockEvents = true;
		bool m_ScenePlay = false;
		SceneFrame m_Scene;
	};
}