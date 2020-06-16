#pragma once

#ifdef T_PLATFORM_WINDOWS

extern Tunti::Application* Tunti::CreateApplication();

int main(int argc, char** argv)
{
	Tunti::Log::Init();
	T_CORE_WARN("Initialized Log!");
	T_INFO("Hello!");

	auto app = Tunti::CreateApplication();
	app->Run();
	delete app;
}

#endif