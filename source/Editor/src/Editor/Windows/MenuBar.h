#pragma once

#include "IEditorWindow.h"

namespace editor
{
	class MenuBar : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}