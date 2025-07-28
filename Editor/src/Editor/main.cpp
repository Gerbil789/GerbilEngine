#include "Engine/Core/Log.h"
#include "Editor/Core/Core.h"
#include "Editor/Core/EditorApp.h"

int main(int argc, char** argv)
{
  Engine::Log::Initialize();

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

  // Set working directory to executable dir early
  //std::filesystem::current_path(GetExecutableDir());

  auto exeDir = GetExecutableDir();
  auto projectRoot = exeDir.parent_path().parent_path().parent_path();
  EDITOR_RESOURCES = projectRoot / "Editor" / "resources";
  ENGINE_RESOURCES = projectRoot / "Engine" / "resources";
  ASSETS = projectPath / "Assets";

  ENGINE_PROFILE_BEGIN("Startup", "Profile-Startup.json");
  auto app = new Editor::EditorApp(projectPath);
  ENGINE_PROFILE_END();

  LOG_INFO("--- Initialization complete ---");

  ENGINE_PROFILE_BEGIN("Runtime", "Profile-Runtime.json");
  app->Run();
  ENGINE_PROFILE_END();

  ENGINE_PROFILE_BEGIN("Shutdown", "Profile-Shutdown.json");
  delete app;
  ENGINE_PROFILE_END();
}