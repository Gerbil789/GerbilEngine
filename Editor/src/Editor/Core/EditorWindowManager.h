#pragma once

#include "Engine/Event/Event.h"

namespace Editor::EditorWindowManager
{
	void Initialize();
	void Shutdown();

	void OnUpdate();
	void OnEvent(Engine::Event& e);

	void ResetLayout();

	template<typename T>
	T* GetWindow();
}
