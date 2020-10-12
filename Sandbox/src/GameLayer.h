#pragma once
#include "Doge.h"

namespace Sandbox
{
	class GameLayer : public Doge::Layer
	{
	public:
		GameLayer();
		~GameLayer() = default;

		virtual void OnUpdate(Doge::Timestep ts) override;
		virtual void OnEvent(Doge::Event& e) override;
	private:
		bool OnKeyPress(Doge::KeyPressedEvent& e);
		bool OnMouseButtonPress(Doge::MouseButtonPressedEvent& e);
	private:
		Doge::Scene m_Scene;
		Doge::PerspectiveCameraController m_CameraController;
		bool m_IsMouseVisible = false;
	};
}