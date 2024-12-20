#include "enginepch.h"
#include "Engine/Core/Application.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Shader.h"

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


		RenderCommand::Init();
		Renderer::Init();
		//Renderer2D::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {}

	void Application::Run()
	{
		ENGINE_PROFILE_FUNCTION();
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");
			
			auto now = std::chrono::steady_clock::now();
			Timestep timestep = std::chrono::duration<float>(now - m_LastFrameTime).count();
			m_LastFrameTime = now;

			float currentFPS = (timestep > 0.0f) ? 1.0f / timestep : 0.0f;

			// Update FPS history
			m_FPSHistorySum -= m_FPSHistory[m_FPSHistoryIndex]; // Subtract the oldest FPS value
			m_FPSHistory[m_FPSHistoryIndex] = currentFPS;        // Insert the new FPS value
			m_FPSHistorySum += currentFPS;                       // Add the new FPS value to the sum

			m_FPSHistoryIndex = (m_FPSHistoryIndex + 1) % FrameHistorySize; // Update the index

			// Calculate the average FPS over the last 30 frames
			m_AverageFPS = m_FPSHistorySum / FrameHistorySize;

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
		LOG_INFO("Application Closed");
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
		return true;
	}

}