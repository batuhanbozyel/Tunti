#pragma once
#include "Platform.h"

#ifdef PLATFORM_WINDOWS
extern Tunti::Application* CreateApplication();

int main(int argc, char** argv)
{
	Tunti::Application* app = CreateApplication();

	app->Run();

	delete app;
}
#else
#error Tunti Engine currently supports Windows only!
#endif