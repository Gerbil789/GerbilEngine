#pragma once

#include "EditorWindow.h"
#include "Engine/Core/Application.h"

namespace Editor
{
	class StatisticsWindow : public EditorWindow
	{
	public:
		~StatisticsWindow() = default;

		void OnUpdate(Engine::Timestep ts) override;
	};
}