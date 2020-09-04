#include "TuntiEditor.h"
#include "Doge/Core/EntryPoint.h"

namespace TEditor
{
	TuntiEditor::TuntiEditor()
		: Application("Tunti Editor", Doge::WindowFlag::MaximizedWindow), m_EditorLayer(new EditorLayer)
	{
		PushOverlay(m_EditorLayer);
	}

	TuntiEditor::~TuntiEditor()
	{
		PopOverlay(m_EditorLayer);
	}
}

Doge::Application* CreateApplication()
{
	return new TEditor::TuntiEditor;
}