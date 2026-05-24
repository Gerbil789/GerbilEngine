#pragma once

#include "Engine/Core/Window.h"

namespace Editor
{
	class EditorApp
	{
	public:
		EditorApp();
		~EditorApp();
		void Run();

	private:
		Engine::Window m_Window;
		bool m_Running = true;
	};
}