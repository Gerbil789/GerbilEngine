#include "enginepch.h"
#include "Application.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/Renderer.h"

namespace Engine
{
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1) //TODO: why is this?

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{
		ENGINE_PROFILE_FUNCTION();
		ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = new Window(name, 1600, 900, "Engine/resources/icons/logo.png");
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Input::Initialize();
		GraphicsContext::Initialize();
		Renderer::Initialize();
		AssetManager::Initialize();
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
		GraphicsContext::SetViewport(e.GetWidth(), e.GetHeight());

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