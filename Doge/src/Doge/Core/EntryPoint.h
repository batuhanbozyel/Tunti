#pragma once

extern Doge::Application* CreateApplication();

int main(int argc, char** argv)
{
	Doge::Application* app = CreateApplication();

	app->Run();

	delete app;
}