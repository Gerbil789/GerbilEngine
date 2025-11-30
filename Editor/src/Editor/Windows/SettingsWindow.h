#pragma once

#include "IEditorWindow.h"

namespace Editor
{
	class SettingsWindow : public IEditorWindow
	{
	public:
		~SettingsWindow() = default;

		void OnUpdate() override;
	};
}