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
		static void SaveLayout();

	private:
		static void BeginDockSpace();
	};
}
