#include "Engine/Core/Log.h"
#include "Editor/Core/EditorApp.h"

int main(int argc, char** argv)
{
  Engine::Log::Initialize();

  try
  {
    auto app = Editor::EditorApp({ argc, argv });
    LOG_INFO("--- Editor initialization complete ---");

    app.Run();
  }
  catch (const std::exception& e)
  {
    LOG_CRITICAL("{}", e.what());
    return EXIT_FAILURE;
  }
}