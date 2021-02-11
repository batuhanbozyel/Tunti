#pragma once
#include "Tunti/Core/Layer.h"
#include "SceneFrame.h"

namespace TEditor
{
	class EditorLayer : public Tunti::Layer
	{
	public:
		static EditorLayer* Create();
		virtual ~EditorLayer() = default;

		virtual void OnAttach();
		virtual void OnDetach();

		virtual void OnEvent(Tunti::Event& e) override;
		virtual void OnUpdate() override;

		void StopScenePlay();
		void StartScenePlay();
	protected:
		EditorLayer();

		virtual void ImGuiBeginRender();
		virtual void ImGuiEndRender();
	private:
		void MenuBarView();
		void StatsView();
		void SceneView();

		bool OnKeyPress(Tunti::KeyPressedEvent& e);
	private:
		bool m_ScenePlay = false;
		SceneFrame m_Scene;
		static EditorLayer* s_EditorAPI;
	};
}