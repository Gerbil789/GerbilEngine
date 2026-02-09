#pragma once

#include "Engine/Event/Event.h"

namespace Engine
{
	class Window;
}

namespace Editor
{
	class EditorWindowManager
	{
	public:
		static void Initialize(Engine::Window& window);
		static void Shutdown();

		static void OnUpdate();
		static void OnEvent(Engine::Event& e);

		static void ResetLayout();

	private:
		static void BeginFrame();
		static void EndFrame();

	};
}
