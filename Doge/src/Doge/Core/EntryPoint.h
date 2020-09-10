#pragma once
#include "Platform.h"

#ifdef PLATFORM_WINDOWS
extern Doge::Application* CreateApplication();

int main(int argc, char** argv)
{
	Doge::Application* app = CreateApplication();

	app->Run();

	delete app;
}
#else
#error Tunti Engine currently supports Windows only!
#endif