#include "enginepch.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Input.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Utils/Path.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/EngineContext.h"

namespace Engine
{
	Application::Application(const ApplicationSpecification& specification)
	{
		ENGINE_PROFILE_FUNCTION();

		std::filesystem::current_path(GetExecutableDir()); // Set working directory

		GraphicsContext::Initialize();
		Window::InitializeGLFW();

		WindowSpecification windowSpec;
		windowSpec.title = specification.title;
		windowSpec.width = 1600;
		windowSpec.height = 900;
		windowSpec.iconPath = "Resources/Engine/icons/logo.png";

		m_MainWindow = new Window(windowSpec);
		m_MainWindow->SetEventCallback([this](Event& e) {this->OnEvent(e);});

		Input::Initialize(*m_MainWindow);
		SamplerPool::Initialize();
		RenderGlobals::Initialize();
		Engine::Audio::Initialize();
	}

	Application::~Application()
	{
		Engine::Audio::Shutdown();
		SamplerPool::Shutdown();
		GraphicsContext::Shutdown();
		delete m_MainWindow;
		Window::ShutdownGLFW();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>([this](auto&) {Close(); });
		dispatcher.Dispatch<WindowResizeEvent>([this](auto& e) {OnWindowResize(e); });
	}

	void Application::OnWindowResize(WindowResizeEvent& e)
	{
		m_Minimized = (e.GetWidth() == 0 || e.GetHeight() == 0) ? true : false;
	}

	void Application::Close()
	{
		m_Running = false;
		LOG_INFO("Application closed");
	}
}