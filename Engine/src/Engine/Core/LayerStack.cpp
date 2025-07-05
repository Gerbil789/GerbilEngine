#include "enginepch.h"
#include "Engine/Core/LayerStack.h"

namespace Engine
{
	LayerStack::LayerStack() {}

	LayerStack::~LayerStack()
	{
		ENGINE_PROFILE_FUNCTION();
		for (Layer* layer : m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}


	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (it != m_Layers.begin() + m_LayerInsertIndex)
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
		else
		{
			LOG_WARNING("Layer not found in LayerStack!");
		}
	}
}