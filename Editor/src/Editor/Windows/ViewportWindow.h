#pragma once

#include "IEditorWindow.h"

namespace Editor
{
	class ViewportWindow : public IEditorWindow
	{
	public:
		void Initialize() override;
		void Draw() override;
	};
}