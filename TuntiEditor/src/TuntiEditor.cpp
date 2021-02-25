#include "Tunti.h"
#include "Tunti/Core/EntryPoint.h"

#include "Editor/EditorLayer.h"

namespace TEditor
{
	class TuntiEditor : public Tunti::Application
	{
	public:
		TuntiEditor()
			: Application("Tunti Editor", Tunti::WindowFlag::MaximizedWindow), m_EditorLayer(new EditorLayer)
		{
			PushLayer(m_EditorLayer);
		}
		~TuntiEditor()
		{
			PopLayer(m_EditorLayer);
			delete m_EditorLayer;
		}
	private:
		EditorLayer* m_EditorLayer;
	};
}

Tunti::Application* CreateApplication()
{
	return new TEditor::TuntiEditor;
}