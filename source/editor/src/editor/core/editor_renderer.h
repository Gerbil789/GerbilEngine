#pragma once

namespace engine { struct Window; }

namespace editor
{
	struct EditorRenderer
	{
		static void Initialize(const engine::Window& window);
		static void Shutdown();
		static void Update();

		static void ResetLayout();
		static void SaveLayout();
	};
}
