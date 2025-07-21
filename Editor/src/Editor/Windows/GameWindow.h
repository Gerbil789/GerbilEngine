#pragma once

#include "EditorWindow.h"

namespace Editor
{
	class GameWindow : public EditorWindow
	{
	public:
		GameWindow();
		~GameWindow() = default;

		void OnUpdate(Engine::Timestep ts) override;
	};
}