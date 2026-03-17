#pragma once

#include "Engine/Core/Window.h"
#include <optional>

namespace Editor
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

	class EditorApp
	{
	public:
		EditorApp(const ApplicationCommandLineArgs& args);
		~EditorApp();
		void Run();

	private:
		std::optional<Engine::Window> m_Window;
		bool m_Running = true;
	};
}