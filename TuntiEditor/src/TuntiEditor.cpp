#include "TuntiEditor.h"
#include "Tunti/Core/EntryPoint.h"

namespace TEditor
{
	TuntiEditor::TuntiEditor()
		: Application("Tunti Editor", Tunti::WindowFlag::MaximizedWindow), m_EditorLayer(EditorLayer::Create())
	{
		PushOverlay(m_EditorLayer);
	}

	TuntiEditor::~TuntiEditor()
	{
		PopOverlay(m_EditorLayer);
	}
}

Tunti::Application* CreateApplication()
{
	return new TEditor::TuntiEditor;
}