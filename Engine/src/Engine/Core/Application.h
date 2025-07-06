#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Window.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Core/Input.h"
#include "Engine/Utils/FPSCounter.h"
#include <chrono>

namespace Engine
{
	class Application
	{
	public:
		Application(const std::string& name = "Gerbil Engine App");
		virtual ~Application() = default;

		virtual void Run() = 0;
		virtual void OnEvent(Event& e);

		static Application& Get() { return *s_Instance; }
		static GraphicsContext* GetGraphicsContext() { return s_GraphicsContext; }

		Window& GetWindow() { return *m_Window; }
		float GetFPS() const { return m_FPSCounter.GetAverageFPS(); }

		void Close();

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	protected:
		static Application* s_Instance;
		static GraphicsContext* s_GraphicsContext;
		Scope<Window> m_Window;

		bool m_Running = true;
		bool m_Minimized = false;

		FPSCounter m_FPSCounter { 30 };
		std::chrono::steady_clock::time_point m_LastFrameTime;
	};
}

