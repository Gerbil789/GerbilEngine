#include "Game.h"
#include "Engine/Core/Time.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Core/Input.h"
#include "Engine/Utility/Path.h"
#include "Engine/Core/Scene.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Core/Project.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Core/Components.h"
#include "Engine/Core/Log.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Configuration.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/Graphics/Font.h"
#include "Engine/System/CameraSystem.h"
#include "Editor/Core/EditorApp.h"

#include "SpinComponent.h"

#ifdef EDITOR
Editor::EditorApp editor;
#endif

uint32_t m_Width = 1280;
uint32_t m_Height = 720;

Engine::Renderer m_Renderer;
wgpu::Texture m_DepthTexture;

static void UpdateSize(Engine::Window& window)
{
	wgpu::Surface surface = window.GetSurface();
	wgpu::SurfaceTexture surfaceTexture;

	surface.GetCurrentTexture(&surfaceTexture);
	if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal)
	{
		LOG_ERROR("Failed to get current surface texture. status: {}", (int)surfaceTexture.status);
		return;
	}

	wgpu::Texture texture = surfaceTexture.texture;

	if (texture.GetWidth() != m_Width || texture.GetHeight() != m_Height || !m_DepthTexture)
	{
		m_Width = texture.GetWidth();
		m_Height = texture.GetHeight();

		if (m_Width > 0 && m_Height > 0)
		{
			m_Renderer.SetSize(static_cast<float>(m_Width), static_cast<float>(m_Height));

			wgpu::TextureDescriptor depthDesc;
			depthDesc.label = "RendererDepthTexture";
			depthDesc.dimension = wgpu::TextureDimension::e2D;
			depthDesc.format = wgpu::TextureFormat::Depth24Plus;
			depthDesc.mipLevelCount = 1;
			depthDesc.sampleCount = 1;
			depthDesc.size = { m_Width, m_Height, 1 };
			depthDesc.usage = wgpu::TextureUsage::RenderAttachment;

			m_DepthTexture = Engine::GraphicsContext::GetDevice().CreateTexture(&depthDesc);
		}
	}

	if (m_Width == 0 || m_Height == 0) return;

	{
		wgpu::TextureViewDescriptor colorViewDesc;
		colorViewDesc.label = "RendererColorTextureView";
		colorViewDesc.dimension = wgpu::TextureViewDimension::e2D;
		colorViewDesc.format = Engine::GraphicsContext::GetSurfaceFormat();
		colorViewDesc.baseMipLevel = 0;
		colorViewDesc.mipLevelCount = 1;
		colorViewDesc.baseArrayLayer = 0;
		colorViewDesc.arrayLayerCount = 1;

		wgpu::TextureView targetColorView = texture.CreateView(&colorViewDesc);

		if (!targetColorView)
		{
			LOG_ERROR("Failed to create color texture view for surface texture");
			return;
		}

		m_Renderer.SetColorTarget(targetColorView);
	}

	{
		wgpu::TextureViewDescriptor depthViewDesc;
		depthViewDesc.label = "RendererDepthTextureView";
		depthViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		depthViewDesc.dimension = wgpu::TextureViewDimension::e2D;
		depthViewDesc.format = wgpu::TextureFormat::Depth24Plus;
		depthViewDesc.baseMipLevel = 0;
		depthViewDesc.mipLevelCount = 1;
		depthViewDesc.baseArrayLayer = 0;
		depthViewDesc.arrayLayerCount = 1;

		wgpu::TextureView targetDepthView = m_DepthTexture.CreateView(&depthViewDesc);

		if (!targetDepthView)
		{
			LOG_ERROR("Failed to create depth texture view");
			return;
		}

		m_Renderer.SetDepthTarget(targetDepthView);

	}
}

GameApp::GameApp()
{
	std::filesystem::path cwd = std::filesystem::current_path();
	std::filesystem::path projectDir = cwd / "projects/TestProject/project.json";

	Engine::Project::Load(projectDir);
	const Engine::Project& project = Engine::Project::GetActive();

	Engine::GraphicsContext::Initialize();
	GLFW::Initialize();

	m_Window.Initialize({ std::format("Game - {}", Engine::Configuration) , m_Width, m_Height, "resources/icons/logo.png" });
	m_Window.SetEventCallback([](auto& e) {Engine::EventBus::Publish(e); });

	Engine::AssetManager::Initialize(project.GetProjectDirectory());

	Engine::Input::SetActiveWindow(*m_Window.GetNativeWindow());
	Engine::Audio::Initialize();
	Engine::FontManager::Initialize();

	Engine::SceneManager::SetActiveScene(project.GetDefaultScene());

#ifndef EDITOR
	m_Renderer.Initialize();
	m_Renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Shadow | Engine::RenderPassType::Opaque);

	UpdateSize(m_Window);

	auto& scene = Engine::AssetManager::GetAsset(Engine::SceneManager::GetActiveScene());
	auto& registry = scene.GetRegistry();
	entt::entity cameraEntity = registry.view<Engine::CameraComponent, Engine::PrimaryCameraTag>(entt::exclude<Engine::EditorTag>).front();
	auto& cc = registry.get<Engine::CameraComponent>(cameraEntity);
	auto& tc = registry.get<Engine::TransformComponent>(cameraEntity);
	m_Renderer.SetCamera(cc, tc);

	registry.emplace_or_replace<Engine::TransformDirty>(cameraEntity);
	registry.emplace_or_replace<Engine::CameraProjectionDirty>(cameraEntity);
	registry.emplace_or_replace<Engine::CameraViewDirty>(cameraEntity);
#else
	editor.Initialize(m_Window);
#endif

	Engine::EventBus::Subscribe<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); return false; });

	LOG_INFO("--- Game initialization complete ---");
}

GameApp::~GameApp()
{
#ifdef EDITOR
	editor.Shutdown();
#endif

	Engine::Runtime::Stop();
	Engine::Audio::Shutdown();
	m_Window.Shutdown();
	GLFW::Shutdown();
	Engine::GraphicsContext::Shutdown();
}

void GameApp::Run()
{
#ifndef EDITOR
	Engine::Runtime::Start();
#endif

	SpinSystem spinSystem;

	auto& reg = Engine::AssetManager::GetAsset(Engine::SceneManager::GetActiveScene()).GetRegistry();

	auto view = reg.view<Engine::TransformComponent>();


	float speed = 1.0f;
	for(auto entity : view)
	{
		auto& spin = reg.emplace<SpinComponent>(entity);
		spin.speed = speed++;
	}

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


		auto& scene = Engine::AssetManager::GetAsset(Engine::SceneManager::GetActiveScene());
		Engine::TransformSystem::Update(scene);

		UpdateSize(m_Window);

		spinSystem.update(scene.GetRegistry(), Engine::Time::DeltaTime());

#ifdef EDITOR
		editor.Update();
#else
		Engine::Runtime::Update();

		auto& registry = scene.GetRegistry();

		entt::entity cameraEntity = registry.view<Engine::PrimaryCameraTag>().front();

		registry.emplace_or_replace<Engine::TransformDirty>(cameraEntity);
		registry.emplace_or_replace<Engine::CameraProjectionDirty>(cameraEntity);
		registry.emplace_or_replace<Engine::CameraViewDirty>(cameraEntity);

		m_Renderer.RenderScene(scene);

		wgpu::Surface surface = m_Window.GetSurface();
		surface.Present();
#endif
	}
}