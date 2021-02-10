#include "pch.h"
#include "LayerStack.h"

namespace Doge
{
	LayerStack::~LayerStack()
	{
		while (!m_Layers.empty())
		{
			Layer* layer = *m_Layers.begin();
			layer->OnDetach();
			m_Layers.erase(m_Layers.begin());
		}

		while (!m_Overlays.empty())
		{
			Layer* overlay = *m_Overlays.begin();
			overlay->OnDetach();
			m_Overlays.erase(m_Overlays.begin());
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.push_back(layer);
		layer->OnAttach();
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Overlays.push_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		const auto& it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.begin())
		{
			layer->OnDetach();
			m_Layers.erase(it);
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		const auto& it = std::find(m_Overlays.begin(), m_Overlays.end(), overlay);
		if (it != m_Layers.end())
		{
			overlay->OnDetach();
			m_Overlays.erase(it);
		}
	}

	void LayerStack::OnStart()
	{
		for (Layer* layer : m_Layers)
			layer->OnStart();

		for (Layer* overlay : m_Overlays)
			overlay->OnStart();
	}

	void LayerStack::OnUpdate()
	{
		for (Layer* layer : m_Layers)
			layer->OnUpdate();

		for (Layer* overlay : m_Overlays)
			overlay->OnUpdate();
	}

}