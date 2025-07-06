#include "enginepch.h"
#include "Application.h"
#include "Engine/Renderer/RenderCommand.h"

namespace Engine
{
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;
	GraphicsContext* Application::s_GraphicsContext = nullptr;

	Application::Application(const std::string& name)
	{
		ENGINE_PROFILE_FUNCTION();
		ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = new Window(name);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		s_GraphicsContext = new GraphicsContext();
		s_GraphicsContext->Init();
	}


	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if(e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		RenderCommand::SetViewport(0, 0,e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::Close()
	{
		m_Running = false;
		LOG_INFO("Application Closed");
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
		return true;
	}

}