#include "enginepch.h"
#include "Engine/Core/Application.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Scene/Material.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Mesh.h"

#include <GLFW/glfw3.h> // TEMP

namespace Engine
{
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	
	Application::Application(const std::string& name)
	{
		ENGINE_PROFILE_FUNCTION();

		ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		AssetManager::RegisterFactory<Texture2D>(std::make_unique<Texture2DFactory>());
		AssetManager::RegisterFactory<Material>(std::make_unique<MaterialFactory>());
		AssetManager::RegisterFactory<Scene>(std::make_unique<SceneFactory>());
		AssetManager::RegisterFactory<Mesh>(std::make_unique<MeshFactory>());
	}

	Application::~Application() {}

	void Application::Run()
	{
		ENGINE_PROFILE_FUNCTION();
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");
			
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			fps = 1 / timestep;
			if (!m_Minimized)
			{
				{
					ENGINE_PROFILE_SCOPE("LayerStack OnUpdate");
					for (Layer* layer : m_LayerStack)
					{
						layer->OnUpdate(timestep);
					}
				}
			}

			{
				ENGINE_PROFILE_SCOPE("ImGuiLayer OnUpdate");
				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
				{
					layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}
			

			m_Window->OnUpdate();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}




	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if(e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}


	void Application::Close()
	{
		m_Running = false;
		ENGINE_LOG_INFO("Application Closed");
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
		return true;
	}

}