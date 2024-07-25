#include "enginepch.h"
#include "Engine/Imgui/ImGuiLayer.h"
#include "imgui.h"
#include "ImGuizmo.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "Engine/Core/Application.h"

#include "Engine/Utils/Color.h"

//temp
#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace Engine
{
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	ImGuiLayer::~ImGuiLayer() {}


	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Regular.ttf", 18.0f);

		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();


		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}



	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	static ImVec4 ColorConvert(const glm::vec4& color)
	{
		return ImVec4(color.r, color.g, color.b, color.a);
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ColorConvert(glm::vec4(0.12f, 0.12f, 0.12f, 1.0f));

		// Headers
		colors[ImGuiCol_Header] = ColorConvert(Color::Gray * glm::vec4(0.5f, 0.5, 0.5, 1.0f));
		colors[ImGuiCol_HeaderHovered] = ColorConvert(Color::Gray * glm::vec4(0.5f, 0.5, 0.5, 1.0f));
		colors[ImGuiCol_HeaderActive] = ColorConvert(Color::Gray * glm::vec4(0.5f, 0.5, 0.5, 1.0f));

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ColorConvert(Color::DarkGray * glm::vec4(0.3f, 0.3, 0.3, 1.0f));
		colors[ImGuiCol_FrameBgHovered] = ColorConvert(Color::DarkGray * glm::vec4(0.3f, 0.3, 0.3, 1.0f));
		colors[ImGuiCol_FrameBgActive] = ColorConvert(Color::DarkGray * glm::vec4(0.3f, 0.3, 0.3, 1.0f));

		// Tabs
		//colors[ImGuiCol_Tab] = ColorConvert(Color::Green);
		//colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		//colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ColorConvert(Color::DarkGray * glm::vec4(0.5f, 0.5, 0.5, 1.0f));
		colors[ImGuiCol_TitleBgActive] = ColorConvert(Color::DarkGray * glm::vec4(0.5f, 0.5, 0.5, 1.0f));
		colors[ImGuiCol_TitleBgCollapsed] = ColorConvert(Color::DarkGray * glm::vec4(0.5f, 0.5, 0.5, 1.0f));

		// Top bar
		colors[ImGuiCol_MenuBarBg] = ColorConvert(Color::DarkGray * glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));



	}

	void ImGuiLayer::OnImGuiRender()
	{
		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);
	}



}