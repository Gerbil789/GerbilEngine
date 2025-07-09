#include "enginepch.h"
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
    std::filesystem::path cwd = std::filesystem::current_path();
    projectPath = cwd / "../Projects/TestProject"; 
    projectPath = std::filesystem::weakly_canonical(projectPath);
  }

  ENGINE_PROFILE_BEGIN("Startup", "Profile-Startup.json");
  auto app = new Editor::EditorApp(projectPath);
  ENGINE_PROFILE_END();

  ENGINE_PROFILE_BEGIN("Runtime", "Profile-Runtime.json");
  app->Run();
  ENGINE_PROFILE_END();

  ENGINE_PROFILE_BEGIN("Shutdown", "Profile-Shutdown.json");
  delete app;
  ENGINE_PROFILE_END();
}