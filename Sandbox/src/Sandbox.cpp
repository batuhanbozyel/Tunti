#include "Tunti.h"

class SandBox : public Tunti::Application
{
public:
	SandBox()
	{

	}

	~SandBox()
	{

	}
};

Tunti::Application* Tunti::CreateApplication()
{
	return new SandBox();
}