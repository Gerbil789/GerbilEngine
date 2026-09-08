#pragma once

#include "Engine/Core/Application.h"

namespace editor
{
	class Editor : public engine::Application
	{
	public:
		Editor(const engine::Application::Configuration& config);
		~Editor();
		void Run();
		void Update();
	};
}