#pragma once

#include "Engine/Core/Window.h"
#include <optional>

namespace Template
{

	class TemplateApp
	{
	public:
		TemplateApp();
		~TemplateApp();
		void Run();

	private:
		std::optional<Engine::Window> m_Window;
		bool m_Running = true;
	};
}