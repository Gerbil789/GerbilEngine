#pragma once

#include "Editor/Windows/IEditorWindow.h"

namespace editor
{
	class EntityInspectorWindow : public IEditorWindow
	{
	public:
		void Draw() override; 
	};
}