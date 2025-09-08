#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Event/Event.h"

namespace Editor
{
	class EditorWindow
	{
	public:
		virtual ~EditorWindow() = default;

		virtual void OnUpdate(Engine::Timestep ts) = 0;
		virtual void OnEvent(Engine::Event& event) {}

		bool IsVisible() const { return m_IsVisible; }
		void SetVisible(bool visible) { m_IsVisible = visible; }

	protected:
		bool m_IsVisible = false;

	};
}
