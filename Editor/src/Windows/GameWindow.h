#pragma once

#include "EditorWindow.h"

namespace Engine
{
	class GameWindow : public EditorWindow
	{
	public:
		GameWindow(EditorContext* context);
		~GameWindow() = default;

		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;
	};
}