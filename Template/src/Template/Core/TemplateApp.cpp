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
#include "Engine/Graphics/Camera.h"

namespace Template
{
	uint32_t m_Width = 1600;
	uint32_t m_Height = 900;


	static void UpdateSize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;


		Engine::Scene& activeScene = Engine::SceneManager::GetActiveScene();
		Engine::Camera* camera = activeScene.GetActiveCamera().Get<Engine::CameraComponent>().camera;
		camera->SetAspectRatio(static_cast<float>(m_Width) / static_cast<float>(m_Height));

		if (m_Width > 0 && m_Height > 0)
		{
			wgpu::Extent3D size = { m_Width, m_Height, 1 };
			// Color
			{
				wgpu::TextureDescriptor desc;
				desc.label = { "RendererColorTexture", WGPU_STRLEN };
				desc.dimension = wgpu::TextureDimension::_2D;
				desc.format = wgpu::TextureFormat::RGBA8Unorm;
				desc.size = size;
				desc.mipLevelCount = 1;
				desc.sampleCount = 1;
				desc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
				wgpu::Texture colorTexture = Engine::GraphicsContext::GetDevice().createTexture(desc);

				wgpu::TextureViewDescriptor view;
				view.label = { "RendererColorTextureView", WGPU_STRLEN };
				view.dimension = wgpu::TextureViewDimension::_2D;
				view.format = desc.format;
				view.baseMipLevel = 0;
				view.mipLevelCount = 1;
				view.baseArrayLayer = 0;
				view.arrayLayerCount = 1;
				Engine::g_Renderer.SetColorTarget(colorTexture.createView(view));
			}

			// Depth
			{
				wgpu::TextureDescriptor desc;
				desc.label = { "RendererDepthTextureView", WGPU_STRLEN };
				desc.dimension = wgpu::TextureDimension::_2D;
				desc.format = wgpu::TextureFormat::Depth24Plus;
				desc.mipLevelCount = 1;
				desc.sampleCount = 1;
				desc.size = size;
				desc.usage = wgpu::TextureUsage::RenderAttachment;
				desc.viewFormatCount = 1;
				desc.viewFormats = &wgpu::TextureFormat::Depth24Plus;
				wgpu::Texture depthTexture = Engine::GraphicsContext::GetDevice().createTexture(desc);

				wgpu::TextureViewDescriptor view;
				view.aspect = wgpu::TextureAspect::DepthOnly;
				view.baseArrayLayer = 0;
				view.arrayLayerCount = 1;
				view.baseMipLevel = 0;
				view.mipLevelCount = 1;
				view.dimension = wgpu::TextureViewDimension::_2D;
				view.format = wgpu::TextureFormat::Depth24Plus;

				Engine::g_Renderer.SetDepthTarget(depthTexture.createView(view));
			}
		}
	}


	TemplateApp::TemplateApp()
	{
		std::filesystem::path projectDir = "C:/Users/vojta/source/repos/GerbilEngine/Projects/TestProject";
		Engine::Project::Load(projectDir);
		const Engine::Project& project = Engine::Project::GetActive();

		std::filesystem::current_path(GetExecutableDir());

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();
		Engine::AssetManager::Initialize(project.GetProjectDirectory());

		m_Window.emplace(Engine::WindowSpecification{ "Game", m_Width, m_Height, "Resources/Engine/icons/logo.png" });
		m_Window->SetEventCallback([](Engine::Event& e) {Engine::EventBus::Get().Publish(e); });
		Engine::Input::SetActiveWindow(*static_cast<GLFWwindow*>(m_Window->GetNativeWindow()));

		Engine::g_Renderer.Initialize();

		Engine::Audio::Initialize();

		std::filesystem::path dllPath = project.GetProjectDirectory() / "bin/windows/" / BUILD_CONFIG / (project.GetTitle() + ".dll");
		Engine::Runtime::LoadScripts(dllPath);

		Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(project.GetStartSceneID());
		if (&scene) 
		{ 
			Engine::SceneManager::SetActiveScene(scene); 
		}


		Engine::g_Renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Shadow | Engine::RenderPassType::Opaque/* | Engine::RenderPassType::Normal | Engine::RenderPassType::Wireframe*/);

		Engine::Scene& activeScene = Engine::SceneManager::GetActiveScene();

		auto cameras = activeScene.GetEntities<Engine::CameraComponent>();
		if (cameras.empty())
		{
			throw std::runtime_error("No camera found in the scene. Please add a camera entity with a CameraComponent.");
		}

		activeScene.SetActiveCamera(cameras[0]);



		Engine::Entity cameraEntity = activeScene.GetActiveCamera();
		Engine::g_Renderer.SetCamera(cameraEntity.Get<Engine::CameraComponent>().camera);

		UpdateSize(m_Width, m_Height);

		Engine::EventBus::Get().Subscribe<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); });
	}

	TemplateApp::~TemplateApp()
	{
		Engine::Runtime::Stop();

		Engine::Audio::Shutdown();
		m_Window.reset();
		GLFW::Shutdown();
	}

	void TemplateApp::Run()
	{
		Engine::Runtime::Start();

		while (m_Running)
		{
			Engine::Time::BeginFrame();				// update delta time and FPS counters
			Engine::Input::Update();					// poll input events
			Engine::Audio::Update();					// release finished audio voices back to pool

			if (m_Window->IsMinimized()) continue;
			Engine::Runtime::Update();			// update game runtime (scripts, audio listener, etc...)

			wgpu::Surface surface = *static_cast<wgpu::Surface*>(m_Window->GetSurface());
			wgpu::SurfaceTexture surfaceTexture;

			surface.getCurrentTexture(&surfaceTexture);
			if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal)
			{
				LOG_ERROR("Failed to get current surface texture. status: {}", (int)surfaceTexture.status);
				return;
			}

			wgpu::Texture texture = surfaceTexture.texture;

			if (texture.getWidth() != m_Width || texture.getHeight() != m_Height)
			{
				UpdateSize(texture.getWidth(), texture.getHeight());
			}

			wgpu::TextureView targetView = texture.createView();

			if (!targetView)
			{
				LOG_ERROR("Failed to create texture view for surface texture");
				return;
			}

			Engine::g_Renderer.SetColorTarget(targetView);
			Engine::g_Renderer.RenderScene(&Engine::SceneManager::GetActiveScene());

			surface.present();
		}
	}
}