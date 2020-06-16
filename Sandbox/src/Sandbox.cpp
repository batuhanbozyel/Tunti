#include "Tunti.h"

class ExampleLayer : public Tunti::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Tunti::Input::IsKeyPressed(T_KEY_E))
		{
			Tunti::Application::Get().GetWindow().SetVSync(true);
		}
		if (Tunti::Input::IsKeyPressed(T_KEY_D))
		{
			Tunti::Application::Get().GetWindow().SetVSync(false);
		}
	}

	void OnEvent(Tunti::Event& event) override
	{
	}
};

class SandBox : public Tunti::Application
{
public:
	SandBox()
	{
		PushLayer(new ExampleLayer());
	}

	~SandBox()
	{

	}
};

Tunti::Application* Tunti::CreateApplication()
{
	return new SandBox();
}