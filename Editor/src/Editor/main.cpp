#include "Engine/Core/Log.h"
#include "Editor/Core/EditorApp.h"

int main(int argc, char** argv)
{
  Engine::Log::Initialize();

  try
  {
    ENGINE_PROFILE_BEGIN("Initialization", "profile_init.json");
    auto app = Editor::CreateApp({ argc, argv });
    ENGINE_PROFILE_END();

    ENGINE_PROFILE_BEGIN("Runtime", "profile_runtime.json");
    app.Run();
    ENGINE_PROFILE_END();

    ENGINE_PROFILE_BEGIN("Shutdown", "profile_shutdown.json");
		app.Shutdown();
    ENGINE_PROFILE_END();
  }
  catch (const std::exception& e)
  {
    LOG_CRITICAL("Fatal error: {0}", e.what());
    return EXIT_FAILURE;
	}
}