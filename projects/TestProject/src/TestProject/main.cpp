#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#ifdef EDITOR
#include "Editor/Editor.h"
#endif

#include "Engine/Core/Scene.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/SceneManager.h"
#include "SpinComponent.h"

int main()
{
	const engine::Application::Configuration config
	{
		.projectPath = "projects/TestProject",
		.windowSpec = { "Game", 1280, 720 },
		.startupScene = engine::Scene{ 10911691135274866464u }
	};

	try
	{
#ifdef EDITOR
		editor::Editor app(config);
#else
		engine::Application app(config);
#endif

		auto& scene = engine::AssetManager::GetAsset(engine::SceneManager::GetActiveScene());
		scene.RegisterSystem<SpinSystem>();

		auto& registry = scene.GetRegistry();
		for (auto entity : registry.view<engine::TransformComponent>())
		{
			registry.emplace_or_replace<SpinComponent>(entity);
		}

		app.Run();
	}
	catch (const std::exception& e)
	{
		LOG_CRITICAL("{}", e.what());
		return EXIT_FAILURE;
	}
}