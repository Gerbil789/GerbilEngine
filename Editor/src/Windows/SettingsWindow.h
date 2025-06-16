#pragma once

#include "EditorWindow.h"

namespace Engine
{
	class SettingsWindow : public EditorWindow
	{
	public:
		SettingsWindow(EditorContext* context) : EditorWindow(context) {}
		~SettingsWindow() = default;

		void OnImGuiRender() override;
	};
}