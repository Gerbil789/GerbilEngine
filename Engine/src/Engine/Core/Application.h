#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Window.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Core/Input.h"
#include "Engine/Utils/FPSCounter.h"
#include <chrono>

namespace Engine
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			ASSERT(index < Count, "Bad command line arguments");
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string title = "Gerbil Application";
		std::filesystem::path workingDirectory;
		ApplicationCommandLineArgs args;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application() = default;

		virtual void Run() = 0;
		virtual void OnEvent(Event& e);

		static Application& Get() { return *s_Instance; }
		static Window& GetWindow() { return *s_Instance->m_Window; }

		float GetFPS() const { return m_FPSCounter.GetAverageFPS(); } //TODO: handle FPS somewhere else

		void Close();

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	protected:
		static Application* s_Instance;
		Window* m_Window;

		bool m_Running = true;
		bool m_Minimized = false;

		FPSCounter m_FPSCounter { 30 };
		std::chrono::steady_clock::time_point m_LastFrameTime;
	};
}

