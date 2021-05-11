#pragma once
#include "Tunti/Events/Event.h"

namespace Tunti
{
	class Layer
	{
	public:
		Layer() {}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnStart() {}
		virtual void OnUpdate(double dt) {}
		virtual void OnEvent(Event& e) {}
	};
}