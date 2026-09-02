#pragma once

namespace engine { class Window; }

namespace editor
{
	class EditorWindowManager
	{
	public:
		static void Initialize(const engine::Window& window);
		static void Shutdown();
		static void Update();

		static void ResetLayout();
		static void SaveLayout();

	private:
		static void BeginDockSpace();
	};
}
