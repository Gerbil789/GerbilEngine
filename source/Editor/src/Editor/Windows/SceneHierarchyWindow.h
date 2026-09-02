#pragma once

#include "IEditorWindow.h"

namespace editor
{
	class SceneHierarchyWindow : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}