#include "enginepch.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"
#include "Editor/Core/EditorApp.h"

int main(int argc, char** argv)
{
  Engine::Log::Init();

  std::filesystem::path projectPath;

  if (argc > 1) 
  {
    projectPath = argv[1];
  }
  else
  {
#ifdef DEBUG
    std::filesystem::path cwd = std::filesystem::current_path();
    projectPath = cwd / "../Projects/TestProject"; 
    projectPath = std::filesystem::weakly_canonical(projectPath);
#elif
		LOG_ERROR("No project path provided.");
    return -1;
#endif
  }

  ENGINE_PROFILE_BEGIN("Startup", "GerbilProfile-Startup.json");
  auto app = new Editor::EditorApp(projectPath);
  ENGINE_PROFILE_END();

  ENGINE_PROFILE_BEGIN("Runtime", "GerbilProfile-Runtime.json");
  app->Run();
  ENGINE_PROFILE_END();

  ENGINE_PROFILE_BEGIN("Shutdown", "GerbilProfile-Shutdown.json");
  delete app;
  ENGINE_PROFILE_END();
}