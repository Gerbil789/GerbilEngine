#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Events/Event.h"
#include "Editor/Services/EditorServiceRegistry.h"

namespace Editor
{
	class EditorContext;

	class EditorWindow
	{
	public:
		EditorWindow(EditorContext* context) : m_Context(context) {};
		virtual ~EditorWindow() = default;

		virtual void OnUpdate(Engine::Timestep ts) {}
		virtual void OnImGuiRender() = 0;
		virtual void OnEvent(Engine::Event& event) {}

		bool IsVisible() const { return m_IsVisible; }
		void SetVisible(bool visible) { m_IsVisible = visible; }

	protected:
		bool m_IsVisible = false;
		EditorContext* m_Context;
	};
}
