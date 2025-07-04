#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Events/Event.h"
#include "Editor/Services/EditorServiceRegistry.h"
#include <imgui.h>

namespace Editor
{
	class EditorWindowManager;

	class EditorWindow
	{
	public:
		EditorWindow(EditorWindowManager* context) : m_Context(context) {};
		virtual ~EditorWindow() = default;

		virtual void OnUpdate(Engine::Timestep ts) = 0;
		virtual void OnEvent(Engine::Event& event) {}

		bool IsVisible() const { return m_IsVisible; }
		void SetVisible(bool visible) { m_IsVisible = visible; }

	protected:
		bool m_IsVisible = false;
		EditorWindowManager* m_Context;
	};
}
