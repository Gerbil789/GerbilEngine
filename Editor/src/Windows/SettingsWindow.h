#pragma once

#include "EditorWindowBase.h"

namespace Engine
{
	class SettingsWindow : public EditorWindowBase
	{
	public:
		SettingsWindow() = default;
		~SettingsWindow() = default;

		void OnImGuiRender() override;
	};
}