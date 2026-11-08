#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#ifdef EDITOR
#include "Editor/Editor.h"
#endif

int main()
{
	const engine::Application::Configuration config
	{
		.projectPath = "projects/TestProject",
		.windowSpec = { "Dev Project", 1280, 720 },
		.startupScene = engine::SceneHandle{ 9116182370241309221u }
	};

	try
	{
		engine::Application app(config);

#ifdef EDITOR
		editor::Editor editor(app);

		while (engine::Application::s_running)
    {
      editor.Update();
    }
#else
		while (engine::Application::s_running)
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