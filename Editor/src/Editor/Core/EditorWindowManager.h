#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Events/Event.h"

namespace Editor::EditorWindowManager
{
	void Initialize();
	void Shutdown();

	void OnUpdate(Engine::Timestep ts);
	void OnEvent(Engine::Event& e);

	void ResetLayout();

	template<typename T>
	T* GetWindow();
}
