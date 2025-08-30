#include "Engine/Core/Log.h"
#include "Engine/Utils/Path.h"
#include "Editor/Core/Core.h"
#include "Editor/Core/EditorApp.h"

int main(int argc, char** argv)
{
  Engine::Log::Initialize();

  std::filesystem::current_path(GetExecutableDir().parent_path().parent_path().parent_path());
	LOG_TRACE("Current working directory: {0}", std::filesystem::current_path());

  std::filesystem::path projectPath;

  if (argc < 2) 
  {
    projectPath = std::filesystem::current_path() / "Projects/TestProject";
		LOG_WARNING("No project path provided. Using default project: {0}", projectPath);
  }
  else
  {
    projectPath = argv[1];
  }

  ENGINE_PROFILE_BEGIN("Startup", "Profile-Startup.json");
  auto app = new Editor::EditorApp(projectPath);
  ENGINE_PROFILE_END();

  LOG_INFO("--- Editor initialization complete ---");

  ENGINE_PROFILE_BEGIN("Runtime", "Profile-Runtime.json");
  app->Run();
  ENGINE_PROFILE_END();

  ENGINE_PROFILE_BEGIN("Shutdown", "Profile-Shutdown.json");
  delete app;
  ENGINE_PROFILE_END();
}