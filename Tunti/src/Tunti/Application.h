#pragma once

#include "Core.h"

namespace Tunti
{
	class TUNTI_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in Client
	Application* CreateApplication();
}

