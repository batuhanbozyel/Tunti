#include "TuntiEditor.h"
#include "Doge/Core/EntryPoint.h"

namespace TEditor
{
	TuntiEditor::TuntiEditor()
		: Application("Tunti Editor"),
		m_EditorLayer(new EditorLayer),
		m_SceneFramebuffer(std::make_unique<Doge::Framebuffer>(Doge::FramebufferSpecification(
			GetActiveWindow()->GetWindowProps().Width,
			GetActiveWindow()->GetWindowProps().Height)))
	{
		PushOverlay(m_EditorLayer);
	}

	TuntiEditor::~TuntiEditor()
	{
		PopOverlay(m_EditorLayer);
	}

	void TuntiEditor::OnUpdate(float dt)
	{
		Doge::Renderer::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Doge::Renderer::Clear();

		m_EditorLayer->Begin();
		m_EditorLayer->OnImGuiRender();
		m_EditorLayer->End();
	}
}

Doge::Application* CreateApplication()
{
	return new TEditor::TuntiEditor;
}