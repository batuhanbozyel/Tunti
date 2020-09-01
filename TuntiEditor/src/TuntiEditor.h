#pragma once
#include "Doge.h"
#include "Editor/EditorLayer.h"

namespace TEditor
{
	class TuntiEditor : public Doge::Application
	{
	public:
		TuntiEditor();
		~TuntiEditor();

		virtual void OnUpdate(float dt) override;
	private:
		EditorLayer* m_EditorLayer;
	};
}

