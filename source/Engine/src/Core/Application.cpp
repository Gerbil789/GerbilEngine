#include "enginepch.h"
#include "Engine/Core/Application.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Graphics/Font.h"
#include "Engine/Core/Time.h"
#include "Engine/Utility/Path.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/System/CameraSystem.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Scene.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Renderer/RenderPass.h"
#include "Engine/Utility/Timer.h"

namespace engine
{
	Window Application::s_window;
	Renderer Application::s_renderer;
	bool Application::s_running = true;

	Application::Application(const Configuration& config)
	{
		Log::Initialize();

		SetupWorkingDirectory();
		Project::Initialize(std::filesystem::current_path() / config.projectPath);

		GraphicsContext::Initialize();
		GLFW::Initialize();

		s_window.Initialize(config.windowSpec);
		s_window.OnClose([](){Application::s_running = false; LOG_INFO("Application closed");});

		AssetRegistry::Load();
		AssetManager::Initialize();

		input::Initialize(s_window);
		Audio::Initialize();
		FontManager::Initialize();

		s_renderer.Initialize(config.windowSpec.width, config.windowSpec.height);
		s_renderer.AddPass<BackgroundPass>();
		s_renderer.AddPass<OpaquePass>();

		SceneManager::SetActiveScene(config.startupScene);
	}

	Application::~Application()
	{
		Audio::Shutdown();
		s_window.Shutdown();
		GLFW::Shutdown();
		GraphicsContext::Shutdown();
	}

	void Application::Update()
	{
		//engine::ScopedTimer timer("Application update time");

		if (s_window.minimized)
		{
			GLFW::WaitEvents();
			Time::BeginFrame();
			return;
		}

		Time::BeginFrame();
		input::Update();
		Audio::Update();

		Scene& scene = SceneManager::GetActiveScene().Get();
		for (const auto& system : scene.GetSystems())
		{
			system();
		}

		entt::registry& registry = scene.registry;
		CameraSystem::Update(registry, s_window.GetAspectRatio());
		TransformSystem::Update(registry, scene.GetRootEntities());
		scene.bvh.Build(registry);
	}

	void Application::Draw()
	{
		Scene& scene = SceneManager::GetActiveScene().Get();
		entt::registry& registry = scene.registry;
		auto camera = scene.GetPrimaryCamera();

		if (s_window.sizeChanged)
		{
			s_renderer.SetSize(s_window.width, s_window.height);
			s_window.sizeChanged = false;

			if(camera)
			{
				entt::entity cameraEntity = scene.GetPrimaryCameraEntity();
				registry.emplace_or_replace<tag::CameraProjectionDirty>(cameraEntity);
			}
		}

		if(camera)
		{
			DrawList drawList = DrawList::Create(scene.registry);
			s_renderer.Render(drawList, camera, s_window.GetSurfaceView());
		}

		s_window.GetSurface().Present();
	}
}
