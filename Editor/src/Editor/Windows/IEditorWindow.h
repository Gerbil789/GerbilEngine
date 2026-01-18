#pragma once

#include "Engine/Event/Event.h"

namespace Editor
{
	class IEditorWindow
	{
	public:
		virtual ~IEditorWindow() = default;

		virtual void Draw() = 0;
		virtual void OnEvent(Engine::Event&) {}

		bool IsVisible() const { return m_IsVisible; }
		void SetVisible(bool visible) { m_IsVisible = visible; }

	protected:
		bool m_IsVisible = false;

	};
}
