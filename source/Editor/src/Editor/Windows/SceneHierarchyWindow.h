#pragma once

#include "IEditorWindow.h"

namespace Editor
{
	class SceneHierarchyWindow : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}