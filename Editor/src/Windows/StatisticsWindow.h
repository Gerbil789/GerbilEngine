#pragma once

#include "EditorWindow.h"
#include "Engine/Core/Application.h"

namespace Engine 
{
	class StatisticsWindow : public EditorWindow
	{
	public:
		StatisticsWindow(EditorContext* context) : EditorWindow(context) {};
		~StatisticsWindow() = default;

		void OnImGuiRender() override;
	};
}