#pragma once

#include "EditorWindow.h"

namespace Editor
{
	class SettingsWindow : public EditorWindow
	{
	public:
		SettingsWindow(EditorContext* context) : EditorWindow(context) {}
		~SettingsWindow() = default;

		void OnImGuiRender() override;
	};
}