#include "enginepch.h"
#include "Engine/Core/Application.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Event/EventBus.h"
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

namespace engine
{
	Window Application::s_Window;
	Renderer Application::s_Renderer;
	bool Application::s_Running = true;

	Application::Application(const Configuration& config)
	{
		Log::Initialize();

		SetupWorkingDirectory();
		Project::Initialize(std::filesystem::current_path() / config.projectPath);

		GraphicsContext::Initialize();
		GLFW::Initialize();

		s_Window.Initialize(config.windowSpec);
		s_Window.SetEventCallback([](auto& e) { EventBus::Publish(e); });

		AssetManager::Initialize();

		Input::Initialize(s_Window.Get());
		Audio::Initialize();
		FontManager::Initialize();

		const std::vector<RenderPass> passes
		{
			pass::Background(),
			pass::Opaque()
		};

		s_Renderer.Initialize(config.windowSpec.width, config.windowSpec.height, passes);

		SceneManager::SetActiveScene(config.startupScene);
	}

	Application::~Application()
	{
		Audio::Shutdown();
		s_Window.Shutdown();
		GLFW::Shutdown();
		GraphicsContext::Shutdown();
	}

	void Application::Run()
	{
		while (s_Running)
		{
			Update();
			Draw();
		}
	}

	void Application::Update()
	{
		if (s_Window.IsMinimized())
		{
			GLFW::WaitEvents();
			Time::BeginFrame();
			return;
		}

		Time::BeginFrame();
		Input::Update();
		Audio::Update();

		auto& scene = AssetManager::GetAsset(SceneManager::GetActiveScene());
		for (const auto& system : scene.GetSystems())
		{
			system();
		}

		CameraSystem::Update(scene.GetRegistry(), s_Window.GetAspectRatio());
		TransformSystem::Update(scene);
	}

	void Application::Draw()
	{
		auto& scene = AssetManager::GetAsset(SceneManager::GetActiveScene());
		const auto& camera = scene.GetPrimaryCamera();

		if (s_Window.SizeChanged())
		{
			s_Renderer.SetSize(s_Window.GetWidth(), s_Window.GetHeight());
			s_Window.ClearResizedFlag();

			entt::entity cameraEntity = scene.GetRegistry().view<PrimaryCameraTag>().front();
			scene.GetRegistry().emplace_or_replace<CameraProjectionDirty>(cameraEntity);
		}

		s_Renderer.RenderScene(scene, camera, s_Window.GetSurfaceView());
		s_Window.GetSurface().Present();
	}
}
