#pragma once

#include "IEditorWindow.h"

namespace Editor
{
	class StatisticsWindow : public IEditorWindow
	{
	public:
		~StatisticsWindow() = default;
		void OnUpdate() override;
	};
}