#include "Engine/Core/Log.h"
#include "Editor/Core/EditorApp.h"

int main(int argc, char** argv)
{
  Engine::Log::Initialize();

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