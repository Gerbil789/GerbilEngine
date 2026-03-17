#pragma once

namespace Engine { class Window; }

namespace Editor
{
	class EditorWindowManager
	{
	public:
		static void Initialize(const Engine::Window& window);
		static void Shutdown();
		static void Update();
		static void ResetLayout();

	private:
		static void BeginFrame();
		static void EndFrame();
	};
}
