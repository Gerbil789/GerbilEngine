#pragma once

#include "IEditorWindow.h"

namespace editor
{
	class StatisticsWindow : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}