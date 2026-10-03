#pragma once

#include "Editor/Windows/IEditorWindow.h"

namespace editor
{
	class AssetInspectorWindow : public IEditorWindow
	{
	public:
		void Initialize() override;
		void Draw() override;
	};
}