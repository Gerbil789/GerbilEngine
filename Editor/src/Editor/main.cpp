#include "Engine/Core/Log.h"
#include "Editor/Core/Core.h"
#include "Editor/Core/EditorApp.h"

int main(int argc, char** argv)
{
  Engine::Log::Initialize();

  // std::filesystem::current_path(GetExecutableDir().parent_path().parent_path().parent_path());
	//LOG_TRACE("Current working directory: {0}", std::filesystem::current_path());

  ENGINE_PROFILE_BEGIN("Startup", "Profile-Startup.json");
	auto app = Editor::CreateApp({ argc, argv });
  ENGINE_PROFILE_END();

  ENGINE_PROFILE_BEGIN("Runtime", "Profile-Runtime.json");
  app->Run();
  ENGINE_PROFILE_END();

  ENGINE_PROFILE_BEGIN("Shutdown", "Profile-Shutdown.json");
  delete app;
  ENGINE_PROFILE_END();
}