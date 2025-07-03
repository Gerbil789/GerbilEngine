#pragma once

#include "EditorWindow.h"

namespace Editor
{
	class GameWindow : public EditorWindow
	{
	public:
		GameWindow(EditorWindowManager* context);
		~GameWindow() = default;

		void OnUpdate(Engine::Timestep ts) override;
	};
}