#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Timestep.h"
#include <any>

namespace Engine {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_LayerName(name) {}
		virtual ~Layer() {}

		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_LayerName; }
	protected:
		std::string m_LayerName;
	};
}
