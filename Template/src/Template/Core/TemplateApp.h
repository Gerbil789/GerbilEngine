#pragma once

#include "Engine/Core/Window.h"
#include <optional>

namespace Template
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			if (index >= Count)
			{
				throw std::out_of_range("Bad command line arguments index");
			}
			return Args[index];
		}
	};

	class TemplateApp
	{
	public:
		TemplateApp(const ApplicationCommandLineArgs& args);
		~TemplateApp();
		void Run();

	private:
		std::optional<Engine::Window> m_Window;
		bool m_Running = true;
	};
}