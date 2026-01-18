#pragma once

#include "IEditorWindow.h"

namespace Editor
{
	class SettingsWindow : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}