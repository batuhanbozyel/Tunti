#pragma once
#include "Doge/Core/Layer.h"
#include "SceneFrame.h"

namespace TEditor
{
	class EditorLayer : public Doge::Layer
	{
	public:
		static EditorLayer* Create();
		virtual ~EditorLayer() = default;

		virtual void OnAttach();
		virtual void OnDetach();

		virtual void OnEvent(Doge::Event& e) override;
		virtual void OnUpdate(float dt) override;

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

		bool OnKeyPress(Doge::KeyPressedEvent& e);
	private:
		bool m_BlockEvents = true;
		bool m_ScenePlay = false;
		SceneFrame m_Scene;
		static EditorLayer* s_EditorAPI;
	};
}