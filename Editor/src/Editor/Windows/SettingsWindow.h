#pragma once

#include "EditorWindow.h"

namespace Editor
{
	class SettingsWindow : public EditorWindow
	{
	public:
		~SettingsWindow() = default;

		void OnUpdate(Engine::Timestep ts) override;
	};
}