#pragma once

#include "Editor/Windows/IEditorWindow.h"

namespace Editor
{
	class MaterialEditorWindow : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}