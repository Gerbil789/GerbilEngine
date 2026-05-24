#pragma once

#include "Engine/Core/Window.h"

namespace Template
{
	class TemplateApp
	{
	public:
		TemplateApp();
		~TemplateApp();
		void Run();

	private:
		Engine::Window m_Window;
		bool m_Running = true;
	};
}