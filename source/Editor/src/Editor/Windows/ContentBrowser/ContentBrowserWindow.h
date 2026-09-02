#pragma once

#include "Editor/Windows/IEditorWindow.h"

namespace editor
{
	class ContentBrowserWindow : public IEditorWindow
	{
	public:
		void Initialize() override;
		void Draw() override;
	};
}