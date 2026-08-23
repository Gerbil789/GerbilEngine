#pragma once

#include "Engine/Core/Window.h"

namespace Editor
{
	class EditorApp
	{
	public:
		EditorApp() = default;

		void Initialize(const Engine::Window& window);
		void Shutdown();
		void Update();
	};
}