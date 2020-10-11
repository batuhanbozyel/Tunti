#pragma once
#include "Doge/Events/Event.h"
#include "Doge/Core/Timestep.h"

namespace Doge
{
	class Layer
	{
	public:
		Layer() {}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& e) {}
	};
}