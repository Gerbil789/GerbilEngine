#include "enginepch.h"
#include "Application.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Input.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/Renderer.h"

namespace Engine
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
	{
		ENGINE_PROFILE_FUNCTION();

		ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		// Set working directory
		if (!specification.workingDirectory.empty())
		{
			std::filesystem::current_path(specification.workingDirectory);
		}

		WindowSpecification windowSpec;
		windowSpec.title = specification.title;
		windowSpec.width = 1600;
		windowSpec.height = 900;
		windowSpec.iconPath = "Resources/Engine/icons/logo.png";

		m_Window = new Window(windowSpec);
		m_Window->SetEventCallback([this](Event& e) {this->OnEvent(e);});

		Input::Initialize();
		GraphicsContext::Initialize();
		Renderer::Initialize();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>([this](auto e) {Close(); });
		dispatcher.Dispatch<WindowResizeEvent>([this](auto e) {OnWindowResize(e); });
	}

	void Application::OnWindowResize(WindowResizeEvent& e)
	{
		if(e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return;
		}

		m_Minimized = false;
		GraphicsContext::SetWindowSize(e.GetWidth(), e.GetHeight());
	}

	void Application::Close()
	{
		m_Running = false;
		LOG_INFO("Application Closed");
	}

	void Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
	}
}