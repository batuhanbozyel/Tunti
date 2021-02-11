#pragma once
#include "Tunti.h"
#include "Editor/EditorLayer.h"

namespace TEditor
{
	class TuntiEditor : public Tunti::Application
	{
	public:
		TuntiEditor();
		~TuntiEditor();
	private:
		EditorLayer* m_EditorLayer;
	};
}

