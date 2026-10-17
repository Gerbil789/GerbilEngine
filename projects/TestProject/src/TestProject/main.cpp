#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#ifdef EDITOR
#include "Editor/Editor.h"
#endif

#include "Engine/Core/Scene.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/SceneManager.h"
#include "SpinComponent.h"

#include "Engine/Core/Log.h"

int main()
{
	const engine::Application::Configuration config
	{
		.projectPath = "projects/TestProject",
		.windowSpec = { "Dev Project", 1280, 720 },
		.startupScene = engine::Scene{ 9116182370241309221u }
	};

	try
	{
		engine::Application app(config);

#ifdef EDITOR
		editor::Editor editor(app);

		while (engine::Application::s_Running)
    {
      editor.Update();
    }
#else
		while (engine::Application::s_Running)
    {
      app.Update();
      app.Draw();
    }
#endif
	}
	catch (const std::exception& e)
	{
		LOG_CRITICAL("{}", e.what());
		return EXIT_FAILURE;
	}
}



// engine::Scene scene = engine::SceneManager::GetActiveScene();
// entt::registry& registry = scene.GetRegistry();

// auto path = engine::AssetRegistry::GetPath(scene);
// LOG_WARNING("SCENE PATH: {}", path);

// scene.RegisterSystem<SpinSystem>();

// for (auto entity : registry.view<engine::TransformComponent>())
// {
// 	registry.emplace_or_replace<SpinComponent>(entity);
// }

// for( auto[entity, name] : registry.view<engine::NameComponent>().each())
// {
// 	LOG_INFO("entity: {}", name.name);
// }