#pragma once

#include "IEditorWindow.h"

namespace Editor
{
	class StatisticsWindow : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}