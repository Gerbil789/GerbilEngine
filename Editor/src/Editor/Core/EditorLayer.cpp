#include "EditorLayer.h"
#include "Editor/Services/EditorServiceRegistry.h"
#include "Engine/Renderer/RenderCommand.h"
#include "imgui/imgui.h"

namespace Editor
{
	using namespace Engine;

	EditorLayer::EditorLayer() : Layer("EditorLayer") 
	{
		m_ImGuiLayer = new ImGuiLayer();
		m_SceneController = CreateScope<SceneController>();
		EditorServiceRegistry::Register<SceneController>(m_SceneController.get());
	}

	void EditorLayer::OnAttach()
	{
		ENGINE_PROFILE_FUNCTION();
		RenderCommand::SetClearColor({ 0.05f, 0.05f, 0.05f, 1.0f });
		SceneManager::CreateScene("NewScene");

		m_EditorContext = CreateRef<EditorContext>();
	}

	void EditorLayer::OnDetach()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Engine::Timestep ts)
	{
		ENGINE_PROFILE_FUNCTION();
		m_EditorContext->OnUpdate(ts); // update windows
	}

	void EditorLayer::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		m_SceneController->OnEvent(e);
		m_EditorContext->OnEvent(e); 
	}

	void EditorLayer::OnImGuiRender()
	{
		ENGINE_PROFILE_FUNCTION();
		m_ImGuiLayer->Begin();

		// imgui dockspace setup
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen) { ImGui::PopStyleVar(2); }

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		// draw windows
		m_EditorContext->OnImGuiRender(); 

		ImGui::End();
		m_ImGuiLayer->End();
	}
}