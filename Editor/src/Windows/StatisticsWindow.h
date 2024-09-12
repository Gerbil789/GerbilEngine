#pragma once

#include "EditorWindowBase.h"
#include "Engine/Core/Application.h"

namespace Engine 
{
	class StatisticsWindow : public EditorWindowBase
	{
	public:
		StatisticsWindow() = default;
		~StatisticsWindow() = default;

		void OnImGuiRender() override;
	};
}