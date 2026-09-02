#pragma once

#include "Editor/Windows/IEditorWindow.h"

namespace editor
{
	class SettingsWindow : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}