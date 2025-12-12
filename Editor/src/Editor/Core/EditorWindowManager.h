#pragma once

#include "Engine/Event/Event.h"

namespace Editor
{
	class EditorWindowManager
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void OnUpdate();
		static void OnEvent(Engine::Event& e);

		static void ResetLayout();

	private:
		static void BeginFrame();
		static void EndFrame();

	};
}
