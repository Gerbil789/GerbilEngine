#pragma once

#include "Engine/Core/Window.h"

namespace Engine
{
	class Event;
	class WindowResizeEvent;
}

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

	struct ApplicationSpecification
	{
		std::string title = "Application";
		ApplicationCommandLineArgs args;
	};

	class EditorApp
	{
	public:
		EditorApp(const ApplicationSpecification& specification);
		~EditorApp();

		void Run();
		void OnEvent(Engine::Event& e);

	private:
		Engine::Window* m_Window;
		bool m_Running = true;
	};
}