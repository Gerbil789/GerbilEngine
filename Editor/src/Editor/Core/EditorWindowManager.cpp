#include "EditorWindowManager.h"
#include "enginepch.h"
#include "Editor/Services/EditorServiceRegistry.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Core/Application.h"
#include "Engine/Utils/Color.h"
#include "Engine/Utils/File.h"
#include "Engine/Renderer/Renderer.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "backends/imgui_impl_wgpu.h"
#include "backends/imgui_impl_glfw.h"

namespace Editor
{
	EditorWindowManager::EditorWindowManager()
	{
		m_ContentBrowserWindow = RegisterWindow<ContentBrowserWindow>();
		m_InspectorWindow = RegisterWindow<InspectorWindow>();
		m_SceneHierarchyWindow = RegisterWindow<SceneHierarchyWindow>();
		m_MaterialWindow = RegisterWindow<MaterialWindow>();
		// m_SettingsWindow = RegisterWindow<SettingsWindow>();
		m_StatisticsWindow = RegisterWindow<StatisticsWindow>();
		m_MeshImportWindow = RegisterWindow<MeshImportWindow>();
		m_ViewportWindow = RegisterWindow<ViewportWindow>();
		m_GameWindow = RegisterWindow<GameWindow>();

		//initialize imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		const std::string imgui_ini = "resources/layouts/imgui.ini";
		if (!std::filesystem::exists(imgui_ini))
		{
			ResetLayout();
			ImGui::SaveIniSettingsToDisk(imgui_ini.c_str());
		}

		io.IniFilename = "resources/layouts/imgui.ini";
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		io.FontDefault = io.Fonts->AddFontFromFileTTF("resources/fonts/roboto/Roboto-Regular.ttf", 18.0f);

		ImGui::StyleColorsDark(); //TODO: make better color palette
		//ImGui::StyleColorsLight();

		ImGui_ImplGlfw_InitForOther(Engine::Application::Get().GetWindow().Get(), true);

		ImGui_ImplWGPU_InitInfo initInfo;
		initInfo.Device = Engine::Application::GetGraphicsContext()->GetDevice();
		initInfo.NumFramesInFlight = 3;
		initInfo.RenderTargetFormat = WGPUTextureFormat_RGBA8Unorm;
		initInfo.DepthStencilFormat = WGPUTextureFormat_Undefined;
		ImGui_ImplWGPU_Init(&initInfo);
	}

	EditorWindowManager::~EditorWindowManager()
	{
		ImGui_ImplWGPU_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorWindowManager::OnUpdate(Engine::Timestep ts)
	{
		ENGINE_PROFILE_FUNCTION();
		BeginFrame();

		m_MenuBar.OnUpdate();
		for (auto& window : m_Windows)
		{
			window->OnUpdate(ts);
		}

#ifdef DEBUG
		bool showDemo = true;
		ImGui::ShowDemoWindow(&showDemo);
#endif

		EndFrame();
	}

	void EditorWindowManager::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		for (auto& window : m_Windows)
		{
			window->OnEvent(e);
		}

		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(Engine::EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(Engine::EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void EditorWindowManager::BeginFrame()
	{
		ImGui_ImplWGPU_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGuizmo::BeginFrame(); //TODO: must be here?

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

	void EditorWindowManager::EndFrame()
	{
		ImGui::End(); // End dockspace window

		Engine::Application& app = Engine::Application::Get();
		Engine::Window& window = app.GetWindow();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)window.GetWidth(), (float)window.GetHeight());

		// Rendering
		ImGui::Render();

		auto device = app.GetGraphicsContext()->GetDevice();
		auto surface = app.GetGraphicsContext()->GetSurface();
		auto queue = app.GetGraphicsContext()->GetQueue();

		wgpu::SurfaceTexture surfaceTexture;
		surface.getCurrentTexture(&surfaceTexture);
		if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal)
		{
			LOG_ERROR("Failed to get current surface texture: {0}", (int)surfaceTexture.status);
			return;
		}

		// Create a view for this surface texture
		wgpu::TextureViewDescriptor viewDescriptor;
		viewDescriptor.label = { "SurfaceTextureViewDescriptor", strlen("SurfaceTextureViewDescriptor") };
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
		encoderDesc.label = { "CommandEncoderDescriptor", strlen("CommandEncoderDescriptor") };
		WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

		// The attachment part of the render pass descriptor describes the target texture of the pass
		wgpu::RenderPassColorAttachment renderPassColorAttachment = {};
		renderPassColorAttachment.view = targetView;
		renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
		renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
		renderPassColorAttachment.clearValue = WGPUColor{ 0.9, 0.1, 0.2, 1.0 };

		wgpu::RenderPassDescriptor renderPassDesc = {};
		renderPassDesc.label = { "RenderPassDescriptor", strlen("RenderPassDescriptor") };
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &renderPassColorAttachment;


		WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

		ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);

		wgpuRenderPassEncoderEnd(renderPass);
		wgpuRenderPassEncoderRelease(renderPass);

		WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
		cmdBufferDescriptor.nextInChain = nullptr;
		cmdBufferDescriptor.label = { "Command buffer", strlen("Command buffer") };
		WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
		wgpuCommandEncoderRelease(encoder);

		wgpuQueueSubmit(queue, 1, &command);
		wgpuCommandBufferRelease(command);

		wgpuDeviceTick(device);


		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 		//WHAT is this??
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		wgpuTextureViewRelease(targetView);

		wgpuSurfacePresent(surface);
		wgpuDeviceTick(device);


	}

	void EditorWindowManager::ResetLayout()
	{
		const std::string default_ini = "resources/layouts/default.ini";

		auto defaultLayoutContent = Engine::ReadFile(default_ini);
		if (!defaultLayoutContent)
		{
			LOG_ERROR("Failed to load default ImGui layout");
			return;
		}

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::LoadIniSettingsFromMemory(defaultLayoutContent.value().c_str());
		LOG_INFO("ImGui layout reset to default");
	}
}