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
		virtual void OnUpdate() override;
	private:
		std::string OpenFileDialog();
		void Begin();
		void End();
		static void SetDarkThemeColors();
	private:
		Tunti::Scope<EditorAPI> m_EditorAPI = nullptr;
	};
}