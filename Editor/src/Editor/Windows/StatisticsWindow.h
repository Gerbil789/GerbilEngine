#pragma once

#include "IEditorWindow.h"
#include "Engine/Core/Application.h"

namespace Editor
{
	class StatisticsWindow : public IEditorWindow
	{
	public:
		~StatisticsWindow() = default;

		void OnUpdate(Engine::Timestep ts) override;
	};
}