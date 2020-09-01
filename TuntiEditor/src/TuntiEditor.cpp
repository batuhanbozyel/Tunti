#include "TuntiEditor.h"
#include "Doge/Core/EntryPoint.h"

namespace TEditor
{
	TuntiEditor::TuntiEditor()
		: Application("Tunti Editor"), m_EditorLayer(new EditorLayer())
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

		m_EditorLayer->OnUpdate(dt);
	}
}

Doge::Application* CreateApplication()
{
	return new TEditor::TuntiEditor;
}