#include "Engine/Utility/Path.h"
#include "Engine/Core/Log.h"
#include "Editor/Core/EditorApp.h"

int main()
{
	SetupWorkingDirectory();
  Engine::Log::Initialize();

  try
  {
    Editor::EditorApp app;
    app.Run();
  }
  catch (const std::exception& e)
  {
    LOG_CRITICAL("{}", e.what());
    return EXIT_FAILURE;
  }
}