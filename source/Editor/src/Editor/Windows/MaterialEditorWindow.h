#pragma once

#include "Editor/Windows/IEditorWindow.h"

namespace editor
{
	class MaterialEditorWindow : public IEditorWindow
	{
	public:
		void Draw() override;
		void Initialize() override;
	};
}