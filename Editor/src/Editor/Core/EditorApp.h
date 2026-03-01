#pragma once

#include "Engine/Core/Window.h"
#include "Engine/Event/Event.h"
#include "Engine/Event/FileEvent.h"
#include <queue>
#include <mutex>

//namespace Engine
//{
//	class Event;
//}

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
		void OnEvent(Engine::Event& e);

		Engine::Window* m_Window;
		bool m_Running = false;

		std::queue<std::unique_ptr<Engine::FileEvent>> m_FileEventQueue;
		std::mutex m_FileEventMutex;

		void PushFileEvent(std::unique_ptr<Engine::FileEvent> e);
		void ProcessFileEvents();
	};
}