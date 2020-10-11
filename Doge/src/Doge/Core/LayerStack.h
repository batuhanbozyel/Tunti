#pragma once
#include "Layer.h"

namespace Doge
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		void OnUpdate(Timestep ts);

		inline const std::list<Layer*>& GetLayers() const { return m_Layers; }
		inline const std::list<Layer*>& GetOverlays() const { return m_Overlays; }
	private:
		std::list<Layer*> m_Layers;
		std::list<Layer*> m_Overlays;
	};
}