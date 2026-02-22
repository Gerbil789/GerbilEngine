
#include <webgpu/webgpu.hpp>

#include "EditorWindowManager.h"
#include "Engine/Core/Window.h"
#include "Engine/Utility/File.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Editor/Windows/MenuBar.h"
#include "Editor/Windows/ContentBrowser/ContentBrowserWindow.h"
#include "Editor/Windows/Inspector/InspectorWindow.h"
#include "Editor/Windows/SceneHierarchyWindow.h"
#include "Editor/Windows/SettingsWindow.h"
#include "Editor/Windows/StatisticsWindow.h"
#include "Editor/Windows/ViewportWindow.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Windows/PopUp/NewProjectPopupWindow.h"
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

#include "Editor/Core/PopupWindowManager.h"
#include <GLFW/glfw3.h>

//TODO: move to style file or something
static void SetupImGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	style.WindowMenuButtonPosition = ImGuiDir_None;

	// Primary background
	colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);  // #131318
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f); // #131318

	colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);

	// Headers
	colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);

	// Buttons
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.32f, 0.40f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.38f, 0.50f, 1.00f);

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.50f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.38f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

	// Borders
	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.25f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	// Text
	colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);

	// Highlights
	colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.70f, 1.00f, 0.50f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.80f, 1.00f, 0.75f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.90f, 1.00f, 1.00f);

	// Scrollbar
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.55f, 1.00f);

	// Style tweaks
	style.WindowRounding = 5.0f;
	style.FrameRounding = 5.0f;
	style.GrabRounding = 5.0f;
	style.TabRounding = 5.0f;
	style.PopupRounding = 5.0f;
	style.ScrollbarRounding = 5.0f;
	style.WindowPadding = ImVec2(10, 10);
	style.FramePadding = ImVec2(6, 4);
	style.ItemSpacing = ImVec2(8, 6);
	style.PopupBorderSize = 0.f;
}

namespace Editor
{
	static wgpu::Device s_Device;
	static wgpu::Surface s_Surface;
	static wgpu::Queue s_Queue;

	MenuBar m_MenuBar;
	std::vector<IEditorWindow*> m_Windows;
	NewProjectPopupWindow newProjectPopup;

	void EditorWindowManager::Initialize(Engine::Window& window)
	{
		s_Device = Engine::GraphicsContext::GetDevice();
		s_Queue = Engine::GraphicsContext::GetQueue();
		s_Surface = *static_cast<wgpu::Surface*>(window.GetSurface());

		PopupManager::Register(&newProjectPopup);

		//TODO: dont heap allocate the windows
		m_Windows = {
			new SceneHierarchyWindow(),
			new ViewportWindow(),
			new ContentBrowserWindow(),
			new InspectorWindow(),
			new StatisticsWindow()
		};

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		const std::string imgui_ini = "Resources/Editor/layouts/imgui.ini";
		if (!std::filesystem::exists(imgui_ini))
		{
			ResetLayout();
			ImGui::SaveIniSettingsToDisk(imgui_ini.c_str());
		}

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = "Resources/Editor/layouts/imgui.ini";
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows //TODO: this is not working correctly, webgpu backend issue?
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Engine/fonts/roboto/Roboto-Regular.ttf", 18.0f);
		SetupImGuiStyle();

		ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(window.GetNativeWindow()), true);

		ImGui_ImplWGPU_InitInfo initInfo;
		initInfo.Device = s_Device;
		initInfo.RenderTargetFormat = wgpu::TextureFormat::RGBA8Unorm;
		initInfo.DepthStencilFormat = wgpu::TextureFormat::Undefined;
		ImGui_ImplWGPU_Init(&initInfo);
	}

	void EditorWindowManager::Shutdown()
	{
		for (auto* window : m_Windows)
		{
			delete window;
		}
		m_Windows.clear();

		ImGui_ImplWGPU_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorWindowManager::OnUpdate()
	{
		ENGINE_PROFILE_FUNCTION();
		BeginFrame();

		m_MenuBar.Draw();

		PopupManager::Draw();

		for (auto& window : m_Windows)
		{
			window->Draw();
		}

		//bool showDemoWindow = true;
		//ImGui::ShowDemoWindow(&showDemoWindow);

		EndFrame();
	}

	void EditorWindowManager::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		for (auto& window : m_Windows)
		{
			window->OnEvent(e);
		}
	}

	void EditorWindowManager::ResetLayout()
	{
		const std::filesystem::path default_iniPath = "Resources/Editor/layouts/default.ini";
		std::string defaultLayoutContent;

		if(!Engine::ReadFile(default_iniPath, defaultLayoutContent)) return;

		ImGui::LoadIniSettingsFromMemory(defaultLayoutContent.c_str());
		LOG_INFO("ImGui layout reset to default");
	}

	void EditorWindowManager::BeginFrame()
	{
		ImGui_ImplWGPU_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ScopedStyle style
		{
			{ ImGuiStyleVar_WindowRounding, 0.0f },
			{ ImGuiStyleVar_WindowBorderSize, 0.0f },
			{ ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)}
		};

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		// main docking window
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}

	void EditorWindowManager::EndFrame()
	{
		ImGui::End(); // end dockspace window

		ImGui::Render();

		//ImGui::UpdatePlatformWindows();
		//ImGui::RenderPlatformWindowsDefault();

		wgpu::SurfaceTexture surfaceTexture;
		s_Surface.getCurrentTexture(&surfaceTexture);
		if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal)
		{
			LOG_ERROR("Failed to get current surface texture. status: {}", (int)surfaceTexture.status);
			return;
		}

		wgpu::Texture texture = surfaceTexture.texture;
		wgpu::TextureView targetView = texture.createView();

		if (!targetView)
		{
			LOG_ERROR("Failed to create texture view for surface texture");
			return;
		}

		wgpu::CommandEncoderDescriptor encoderDesc {};
		encoderDesc.label = { "ImGuiCommandEncoderDescriptor", WGPU_STRLEN };
		wgpu::CommandEncoder encoder = s_Device.createCommandEncoder(encoderDesc);

		wgpu::RenderPassColorAttachment color = {};
		color.view = targetView;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Clear;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color{ 0.9, 0.1, 0.2, 1.0 };

		wgpu::RenderPassDescriptor passDesc {};
		passDesc.label = { "ImGuiRenderPassDescriptor", WGPU_STRLEN };
		passDesc.colorAttachmentCount = 1;
		passDesc.colorAttachments = &color;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDesc);
		ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
		pass.end();

		wgpu::CommandBufferDescriptor commandBufferDesc {};
		commandBufferDesc.label = { "ImGuiCommandBuffer", WGPU_STRLEN };
		commandBufferDesc.nextInChain = nullptr;
		wgpu::CommandBuffer commandBuffer = encoder.finish(commandBufferDesc);

		s_Queue.submit(1, &commandBuffer);
		s_Surface.present();
	}
}