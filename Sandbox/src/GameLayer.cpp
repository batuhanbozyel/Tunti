#include "GameLayer.h"

namespace Sandbox
{
	GameLayer::GameLayer()
		: m_CameraController(60.0f, Doge::Application::GetActiveWindow()->GetWindowProps().Width,
									Doge::Application::GetActiveWindow()->GetWindowProps().Height)
	{
		ConstructDummyScene();
	}

	void GameLayer::OnUpdate()
	{
		m_CameraController.OnUpdate();
		m_Scene.OnUpdate();
	}

	void GameLayer::OnEvent(Doge::Event& e)
	{
		Doge::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Doge::KeyPressedEvent>(BIND_EVENT_FN(OnKeyPress));
		dispatcher.Dispatch<Doge::MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPress));
	}

	void GameLayer::ConstructDummyScene()
	{
		
	}

	bool GameLayer::OnKeyPress(Doge::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
			case Doge::Key::Escape:
			{
				Doge::Application::GetInstance()->Shutdown();
				return true;
			}
		}
		return false;
	}

	bool GameLayer::OnMouseButtonPress(Doge::MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
			case Doge::Mouse::ButtonRight:
			{
				m_IsMouseVisible ? Doge::Application::DisableCursor() : Doge::Application::EnableCursor();
				m_IsMouseVisible = !m_IsMouseVisible;
				return true;
			}
		}
		return false;
	}
}