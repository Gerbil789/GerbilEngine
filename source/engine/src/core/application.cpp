#include "enginepch.h"
#include "engine/core/application.h"
#include "engine/core/context.h"
#include "engine/core/input.h"
#include "engine/core/scene.h"
#include "engine/core/time.h"
#include "engine/graphics/graphics_context.h"
#include "engine/graphics/renderer/render_pass.h"
#include "engine/graphics/font.h"
#include "engine/asset/asset_registry.h"
#include "engine/asset/asset_manager.h"
#include "engine/audio/audio.h"
#include "engine/system/transform_system.h"
#include "engine/system/camera_system.h"
#include "engine/utility/path.h"
#include "engine/utility/timer.h"

namespace engine
{
	Window Application::s_window;
	Renderer Application::s_renderer;
	bool Application::s_running = true;

	Application::Application(const Configuration& config)
	{
		Log::Initialize();

		SetupWorkingDirectory();

		engine::Context::projectDirectory = std::filesystem::current_path() / config.projectPath;
		engine::Context::assetsDirectory = engine::Context::projectDirectory / "assets";

		graphicsContext::Initialize();
		GLFW::Initialize();

		s_window.Initialize(config.windowSpec);
		s_window.OnClose([](){Application::s_running = false; LOG_INFO("Application closed");});

		assetRegistry::Load();
		assetManager::Initialize();

		input::Initialize(s_window);
		audio::Initialize();
		FontManager::Initialize();

		s_renderer.Initialize(config.windowSpec.width, config.windowSpec.height);
		s_renderer.Addpass<Backgroundpass>();
		s_renderer.Addpass<Opaquepass>();

		engine::Context::SetActiveScene(config.startupScene);
	}

	Application::~Application()
	{
		audio::Shutdown();
		s_window.Shutdown();
		GLFW::Shutdown();
		graphicsContext::Shutdown();
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
		engine::Context::Update(); //delete marked entities

		input::Update();
		audio::Update();

		Scene& scene = engine::Context::activeSceneHandle.Get();
		scene.Update(); // update the game systems

		entt::registry& registry = scene.registry;
		Camerasystem::Update(registry, s_window.GetAspectRatio());
		Transformsystem::Update(scene);
		scene.bvh.Build(registry);
	}

	void Application::Draw()
	{
		Scene& scene = engine::Context::activeSceneHandle.Get();
		entt::registry& registry = scene.registry;
		component::Camera* camera = registry.try_get<component::Camera>(scene.primaryCameraEntity);

		if (s_window.sizeChanged)
		{
			s_renderer.SetSize(s_window.width, s_window.height);
			s_window.sizeChanged = false;

			if(camera)
			{
				registry.emplace_or_replace<tag::CameraProjectionDirty>(scene.primaryCameraEntity);
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
