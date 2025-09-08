#include "enginepch.h"
#include "EditorWindowManager.h"
#include "Engine/Core/Application.h"
#include "Engine/Utils/Color.h"
#include "Engine/Utils/File.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Editor/Elements/MenuBar.h"
#include "Editor/Windows/ContentBrowserWindow.h"
#include "Editor/Windows/InspectorWindow.h"
#include "Editor/Windows/SceneHierarchyWindow.h"
#include "Editor/Windows/MaterialWindow.h"
#include "Editor/Windows/SettingsWindow.h"
#include "Editor/Windows/StatisticsWindow.h"
#include "Editor/Windows/MeshImportWindow.h"
#include "Editor/Windows/ViewportWindow.h"
#include "Editor/Windows/GameWindow.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

void SetupImGuiStyle()
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




namespace Editor::EditorWindowManager
{
	MenuBar m_MenuBar;

	ViewportWindow* m_ViewportWindow = nullptr;
	GameWindow* m_GameWindow = nullptr;
	ContentBrowserWindow* m_ContentBrowserWindow = nullptr;
	InspectorWindow* m_InspectorWindow = nullptr;
	SceneHierarchyWindow* m_SceneHierarchyWindow = nullptr;
	MaterialWindow* m_MaterialWindow = nullptr;
	StatisticsWindow* m_StatisticsWindow = nullptr;
	//SettingsWindow* m_SettingsWindow = nullptr;
	//MeshImportWindow* m_MeshImportWindow = nullptr;

	template<>ViewportWindow* GetWindow<ViewportWindow>() { return m_ViewportWindow; }
	template<>GameWindow* GetWindow<GameWindow>() { return m_GameWindow; }
	template<>ContentBrowserWindow* GetWindow<ContentBrowserWindow>() { return m_ContentBrowserWindow; }
	template<>InspectorWindow* GetWindow<InspectorWindow>() { return m_InspectorWindow; }
	template<>SceneHierarchyWindow* GetWindow<SceneHierarchyWindow>() { return m_SceneHierarchyWindow; }
	template<> MaterialWindow* GetWindow<MaterialWindow>() { return m_MaterialWindow; }
	template<> StatisticsWindow* GetWindow<StatisticsWindow>() { return m_StatisticsWindow; }
	//template<>SettingsWindow* GetWindow<SettingsWindow>() { return m_SettingsWindow; }
	//template<>MeshImportWindow* GetWindow<MeshImportWindow>() { return m_MeshImportWindow; }

	std::vector<EditorWindow*> m_Windows; // For easy iteration

	void Initialize()
	{
		m_ContentBrowserWindow = new ContentBrowserWindow();
		m_InspectorWindow = new InspectorWindow();
		m_SceneHierarchyWindow = new SceneHierarchyWindow();
		m_MaterialWindow = new MaterialWindow();
		m_StatisticsWindow = new StatisticsWindow();
		m_ViewportWindow = new ViewportWindow();
		m_GameWindow = new GameWindow();
		//m_SettingsWindow = new SettingsWindow();
		//m_MeshImportWindow = new MeshImportWindow();

		m_Windows = {
			m_ContentBrowserWindow,
			m_InspectorWindow,
			m_SceneHierarchyWindow,
			m_MaterialWindow,
			m_StatisticsWindow,
			m_ViewportWindow,
			m_GameWindow,
			//m_SettingsWindow,
			//m_MeshImportWindow
		};

		//initialize imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		const std::string imgui_ini = "Editor/resources/layouts/imgui.ini";
		if (!std::filesystem::exists(imgui_ini))
		{
			ResetLayout();
			ImGui::SaveIniSettingsToDisk(imgui_ini.c_str());
		}

		io.IniFilename = "Editor/resources/layouts/imgui.ini"; // IMPORTANT: relative to the current working directory TODO: this is an issue, need to fix it
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Engine/resources/fonts/roboto/Roboto-Regular.ttf", 18.0f);
		//ImGui::StyleColorsDark(); //TODO: make better color palette
		//ImGui::StyleColorsLight();
		SetupImGuiStyle();


		ImGui_ImplGlfw_InitForOther(Engine::Application::GetWindow().GetNativeWindow(), true);

		ImGui_ImplWGPU_InitInfo initInfo;
		initInfo.Device = Engine::GraphicsContext::GetDevice();
		initInfo.NumFramesInFlight = 3;
		initInfo.RenderTargetFormat = WGPUTextureFormat_RGBA8Unorm;
		initInfo.DepthStencilFormat = WGPUTextureFormat_Undefined;
		ImGui_ImplWGPU_Init(&initInfo);
	}

	void Shutdown()
	{
		for (auto* window : m_Windows)
			delete window;

		m_Windows.clear();

		ImGui_ImplWGPU_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void BeginFrame()
	{
		ImGui_ImplWGPU_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

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
	}

	void EndFrame()
	{
		ImGui::End(); // End dockspace window

		Engine::Window& window = Engine::Application::GetWindow(); //TODO: store this in a member variable?
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)window.GetWidth(), (float)window.GetHeight());

		// Draw everything
		ImGui::Render();

		auto device = Engine::GraphicsContext::GetDevice(); //TODO: store this in a member variable?
		auto surface = Engine::GraphicsContext::GetSurface();
		auto queue = Engine::GraphicsContext::GetQueue();

		wgpu::SurfaceTexture surfaceTexture;
		surface.getCurrentTexture(&surfaceTexture);
		if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal)
		{
			LOG_ERROR("Failed to get current surface texture: {0}", (int)surfaceTexture.status);
			return;
		}

		// Create a view for this surface texture
		wgpu::TextureViewDescriptor viewDescriptor;
		viewDescriptor.label = { "SurfaceTextureViewDescriptor", WGPU_STRLEN };
		viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
		viewDescriptor.dimension = WGPUTextureViewDimension_2D;
		viewDescriptor.baseMipLevel = 0;
		viewDescriptor.mipLevelCount = 1;
		viewDescriptor.baseArrayLayer = 0;
		viewDescriptor.arrayLayerCount = 1;
		viewDescriptor.aspect = WGPUTextureAspect_All;
		viewDescriptor.usage = WGPUTextureUsage_RenderAttachment;
		wgpu::TextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);
		wgpuTextureRelease(surfaceTexture.texture);

		if (!targetView)
		{
			LOG_ERROR("Failed to create texture view for surface texture");
			return;
		}

		wgpu::CommandEncoderDescriptor encoderDesc = {};
		encoderDesc.label = { "CommandEncoderDescriptor", WGPU_STRLEN };
		WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

		// The attachment part of the render pass descriptor describes the target texture of the pass
		wgpu::RenderPassColorAttachment renderPassColorAttachment = {};
		renderPassColorAttachment.view = targetView;
		renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
		renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
		renderPassColorAttachment.clearValue = WGPUColor{ 0.9, 0.1, 0.2, 1.0 };

		wgpu::RenderPassDescriptor renderPassDesc = {};
		renderPassDesc.label = { "RenderPassDescriptor", WGPU_STRLEN };
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &renderPassColorAttachment;


		WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

		ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);

		wgpuRenderPassEncoderEnd(renderPass);
		wgpuRenderPassEncoderRelease(renderPass);

		WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
		cmdBufferDescriptor.nextInChain = nullptr;
		cmdBufferDescriptor.label = { "Command buffer", WGPU_STRLEN };
		WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
		wgpuCommandEncoderRelease(encoder);

		wgpuQueueSubmit(queue, 1, &command);
		wgpuCommandBufferRelease(command);

		wgpuDeviceTick(device);


		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) //TODO: WHAT is this??
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		wgpuTextureViewRelease(targetView);

		wgpuSurfacePresent(surface);
		wgpuDeviceTick(device);
	}

	void OnUpdate(Engine::Timestep ts)
	{
		ENGINE_PROFILE_FUNCTION();
		BeginFrame();

		m_MenuBar.OnUpdate();
		for (auto& window : m_Windows)
		{
			window->OnUpdate(ts);
		}

		bool showDemo = true;
		ImGui::ShowDemoWindow(&showDemo);

		EndFrame();
	}

	void OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		for (auto& window : m_Windows)
		{
			window->OnEvent(e);
		}
	}

	void ResetLayout()
	{
		const std::filesystem::path default_iniPath = "Editor/resources/layouts/default.ini";
		std::string defaultLayoutContent;

		if(!Engine::ReadFile(default_iniPath, defaultLayoutContent)) return;

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::LoadIniSettingsFromMemory(defaultLayoutContent.c_str());
		LOG_INFO("ImGui layout reset to default");
	}

	
}