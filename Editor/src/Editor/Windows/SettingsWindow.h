#pragma once

#include "EditorWindow.h"

namespace Editor
{
	class SettingsWindow : public EditorWindow
	{
	public:
		SettingsWindow(EditorWindowManager* context) : EditorWindow(context) {}
		~SettingsWindow() = default;

		void OnUpdate(Engine::Timestep ts) override;
	};
}