#pragma once
#include "Tunti/Core/Layer.h"
#include "EditorAPI.h"

namespace TEditor
{
	class EditorLayer : public Tunti::Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnEvent(Tunti::Event& e) override;
		virtual void OnUpdate(double dt) override;
	private:
		void Begin();
		void End();
	private:
		Tunti::EditorCamera m_EditorCamera;

		Tunti::Scope<EditorAPI> m_EditorAPI = nullptr;
	};
}