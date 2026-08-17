#define WEBGPU_CPP_IMPLEMENTATION

#include "TemplateApp.h"
#include "Engine/Core/Time.h"
#include "Engine/Utility/File.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Script/Script.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Core/Input.h"
#include "Engine/Utility/Path.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Core/Project.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Scene/Components.h"
#include "Engine/Core/Log.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Configuration.h"

namespace Template
{
	uint32_t m_Width = 1600;
	uint32_t m_Height = 900;

	Engine::Renderer m_Renderer;

	static void UpdateSize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0) return;

		m_Width = width;
		m_Height = height;

		//Engine::Scene& activeScene = Engine::AssetManager::GetAsset<Engine::Scene>(Engine::SceneManager::GetActiveScene());
		//Engine::Camera* camera = activeScene.GetActiveCamera();
		//m_Camera->SetAspectRatio(static_cast<float>(m_Width) / static_cast<float>(m_Height));

		wgpu::Extent3D size = { m_Width, m_Height, 1 };

		// Color
		{
			wgpu::TextureDescriptor desc;
			desc.label = "RendererColorTexture";
			desc.dimension = wgpu::TextureDimension::e2D;
			desc.format = Engine::GraphicsContext::GetSurfaceFormat();
			desc.size = size;
			desc.mipLevelCount = 1;
			desc.sampleCount = 1;
			desc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
			wgpu::Texture colorTexture = Engine::GraphicsContext::GetDevice().CreateTexture(&desc);

			wgpu::TextureViewDescriptor view;
			view.label = "RendererColorTextureView";
			view.dimension = wgpu::TextureViewDimension::e2D;
			view.format = desc.format;
			view.baseMipLevel = 0;
			view.mipLevelCount = 1;
			view.baseArrayLayer = 0;
			view.arrayLayerCount = 1;
			m_Renderer.SetColorTarget(colorTexture.CreateView(&view));
		}

		// Depth
		{
			wgpu::TextureDescriptor desc;
			desc.label = "RendererDepthTextureView";
			desc.dimension = wgpu::TextureDimension::e2D;
			desc.format = wgpu::TextureFormat::Depth24Plus;
			desc.mipLevelCount = 1;
			desc.sampleCount = 1;
			desc.size = size;
			desc.usage = wgpu::TextureUsage::RenderAttachment;
			desc.viewFormatCount = 1;
			desc.viewFormats = &desc.format;
			wgpu::Texture depthTexture = Engine::GraphicsContext::GetDevice().CreateTexture(&desc);

			wgpu::TextureViewDescriptor view;
			view.aspect = wgpu::TextureAspect::DepthOnly;
			view.baseArrayLayer = 0;
			view.arrayLayerCount = 1;
			view.baseMipLevel = 0;
			view.mipLevelCount = 1;
			view.dimension = wgpu::TextureViewDimension::e2D;
			view.format = wgpu::TextureFormat::Depth24Plus;

			m_Renderer.SetDepthTarget(depthTexture.CreateView(&view));
		}

	}


	TemplateApp::TemplateApp()
	{
		std::filesystem::path cwd = std::filesystem::current_path();

		std::filesystem::path projectDir = cwd / "Projects/TestProject/project.json"; //TODO: dont hardcode paths

		Engine::Project::Load(projectDir);
		const Engine::Project& project = Engine::Project::GetActive();

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();

		m_Window.Initialize({ std::format("Game - {}", Engine::Configuration) , m_Width, m_Height, "resources/icons/logo.png" });
		m_Window.SetEventCallback([](auto& e) {Engine::EventBus::Publish(e); });

		Engine::AssetManager::Initialize(project.GetProjectDirectory());

		Engine::Input::SetActiveWindow(*m_Window.GetNativeWindow());

		m_Renderer.Initialize();
		m_Renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Shadow | Engine::RenderPassType::Opaque);

		Engine::Audio::Initialize();

		std::filesystem::path dllPath = project.GetProjectDirectory() / "bin/windows/" / Engine::Configuration / (project.GetTitle() + ".dll");
		Engine::Runtime::LoadScripts(dllPath);

		Engine::Uuid id = project.GetDefaultSceneId();
		Engine::SceneManager::SetActiveScene(id);

		Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(Engine::SceneManager::GetActiveScene());
		entt::entity cameraEntity = scene.GetActiveCamera();
		auto& cc = scene.GetRegistry().get<Engine::CameraComponent>(cameraEntity);
		cc.background = Engine::CameraComponent::Background::Skybox;

		UpdateSize(m_Width, m_Height);

		Engine::EventBus::Subscribe<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); return false; });

		LOG_INFO("--- Game initialization complete ---");
	}

	TemplateApp::~TemplateApp()
	{
		Engine::Runtime::Stop();
		Engine::Audio::Shutdown();
		GLFW::Shutdown();
	}

	void TemplateApp::Run()
	{
		Engine::Runtime::Start();

		while (m_Running)
		{
			if (m_Window.IsMinimized())
			{
				GLFW::WaitEvents();
				Engine::Time::BeginFrame();
				continue;
			}


			Engine::Time::BeginFrame();	
			Engine::Input::Update();
			Engine::Audio::Update();
			Engine::PhysicsSystem::Update();
			Engine::Runtime::Update();

			wgpu::Surface surface = m_Window.GetSurface();
			wgpu::SurfaceTexture surfaceTexture;

			surface.GetCurrentTexture(&surfaceTexture);
			if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal)
			{
				LOG_ERROR("Failed to get current surface texture. status: {}", (int)surfaceTexture.status);
				return;
			}

			wgpu::Texture texture = surfaceTexture.texture;

			if (texture.GetWidth() != m_Width || texture.GetHeight() != m_Height)
			{
				UpdateSize(texture.GetWidth(), texture.GetHeight());
			}

			wgpu::TextureView targetView = texture.CreateView();

			if (!targetView)
			{
				LOG_ERROR("Failed to create texture view for surface texture");
				return;
			}

			m_Renderer.SetColorTarget(targetView);
			m_Renderer.RenderScene(Engine::AssetManager::GetAsset<Engine::Scene>(Engine::SceneManager::GetActiveScene()));

			surface.Present();
		}
	}
}