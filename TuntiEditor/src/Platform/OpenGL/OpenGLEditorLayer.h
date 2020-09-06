#pragma once
#include "Editor/EditorLayer.h"

#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

namespace TEditor
{
	class OpenGLEditorLayer : public EditorLayer
	{
	public:
		OpenGLEditorLayer() = default;
		~OpenGLEditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
	private:
		virtual void ImGuiBeginRender() override;
		virtual void ImGuiEndRender() override;
	};
}