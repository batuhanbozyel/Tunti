#pragma once

#include "Tunti/Core/Layer.h"

#include "Tunti/Events/ApplicationEvent.h"
#include "Tunti/Events/KeyEvent.h"
#include "Tunti/Events/MouseEvent.h"

namespace Tunti
{
	class TUNTI_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}