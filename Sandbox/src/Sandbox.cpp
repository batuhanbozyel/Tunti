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
	}

	void OnEvent(Tunti::Event& event) override
	{
		T_TRACE("{0}", event);
	}
};

class SandBox : public Tunti::Application
{
public:
	SandBox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Tunti::ImGuiLayer());
	}

	~SandBox()
	{

	}
};

Tunti::Application* Tunti::CreateApplication()
{
	return new SandBox();
}