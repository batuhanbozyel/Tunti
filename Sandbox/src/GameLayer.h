#pragma once
#include "Doge.h"

namespace Sandbox
{
	class GameLayer : public Doge::Layer
	{
	public:
		GameLayer() = default;
		~GameLayer() = default;

		virtual void OnStart() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Doge::Event& e) override;
	private:
		void ConstructTestScene();

		bool OnKeyPress(Doge::KeyPressedEvent& e);
		bool OnWindowResize(Doge::WindowResizeEvent& e);
		bool OnMouseButtonPress(Doge::MouseButtonPressedEvent& e);
	private:
		Doge::FirstPersonCameraController m_FPSCameraController;
		Doge::Scene m_Scene;

		bool m_MouseVisible = true;
	};
}