#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Window.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Core/LayerStack.h"
#include "Engine/Core/Input.h"
#include "Engine/Utils/FPSCounter.h"
#include <chrono>

#include <webgpu/webgpu.h>

namespace Engine
{
	class Application
	{
	public:
		Application(const std::string& name = "Gerbil Engine App");
		virtual ~Application();

		virtual void Run() = 0;

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		float GetFPS() const { return m_FPSCounter.GetAverageFPS(); }
		static GraphicsContext* GetGraphicsContext() { return s_GraphicsContext; }

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
		LayerStack m_LayerStack;

		FPSCounter m_FPSCounter{ 30 };
		std::chrono::steady_clock::time_point m_LastFrameTime;
	};
}

