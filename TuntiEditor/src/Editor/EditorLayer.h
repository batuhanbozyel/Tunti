#pragma once
#include "Doge/Core/Layer.h"

#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

namespace TEditor
{
	class EditorLayer : public Doge::Layer
	{
	public:
		EditorLayer() = default;
		~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Doge::Event& e) override;

		void OnImGuiRender();

		static void Begin();
		static void End();

		inline void BlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
	};
}