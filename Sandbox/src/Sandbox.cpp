#include "Tunti.h"

#include "imgui.h"

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

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(Tunti::Event& event) override
	{
		if (event.GetEventType() == Tunti::EventType::KeyPressed)
		{
			Tunti::KeyPressedEvent& e = (Tunti::KeyPressedEvent&)event;
			if (e.GetKeyCode() == T_KEY_TAB)
			{
				Tunti::Application::Get().GetWindow().SetVSync(vsync);
				vsync = !vsync;
			}
		}
	}
private:
	bool vsync = false;
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