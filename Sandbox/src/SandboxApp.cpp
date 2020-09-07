#include "Doge.h"
#include "Doge/Core/EntryPoint.h"

#include "GameLayer.h"

namespace Sandbox
{
	class SandboxApp : public Doge::Application
	{
	public:
		SandboxApp()
			: Application("SandboxApp", Doge::WindowFlag::MaximizedWindow), m_GameLayer(new GameLayer)
		{
			PushLayer(m_GameLayer);
		}

		~SandboxApp()
		{
			PopLayer(m_GameLayer);
		}
	private:
		Doge::Layer* m_GameLayer;
	};
}

Doge::Application* CreateApplication()
{
	return new Sandbox::SandboxApp();
}