#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Window.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Core/LayerStack.h"
#include "Engine/Core/Input.h"
#include <chrono>

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

		void Close();

		float m_AverageFPS = 0.0f; // To store the calculated average FPS

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	protected:
		static Application* s_Instance;
		std::unique_ptr<Window> m_Window;

		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		// FPS Calculation
		std::chrono::steady_clock::time_point m_LastFrameTime;
		static constexpr int FrameHistorySize = 30; // Number of frames to average over
		std::array<float, FrameHistorySize> m_FPSHistory = { 0.0f }; // To store FPS values
		int m_FPSHistoryIndex = 0; // To track the current position in the array
		float m_FPSHistorySum = 0.0f; // To keep track of the sum of the FPS values
	};
}

