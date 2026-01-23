#pragma once

#include "Engine/Core/Window.h"
#include "Engine/Event/ApplicationEvent.h"

namespace Engine
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			if(index >= Count)
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

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		virtual void Run() = 0;
		virtual void OnEvent(Event& e);

		static Application& Get() { return *s_Instance; }
		static Window& GetWindow() { return *s_Instance->m_MainWindow; }

		void Close();

	private:
		void OnWindowResize(WindowResizeEvent& e);

	protected:
		static Application* s_Instance;
		Window* m_MainWindow;

		bool m_Running = true;
		bool m_Minimized = false;
	};
}

