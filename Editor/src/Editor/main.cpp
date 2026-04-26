#include "Engine/Core/Log.h"
#include "Editor/Core/EditorApp.h"

int main()
{
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