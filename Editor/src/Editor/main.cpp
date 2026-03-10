#include "Engine/Core/Log.h"
#include "Editor/Core/EditorApp.h"

int main(int argc, char** argv)
{
  Engine::Log::Initialize();

#if defined(_MSC_VER)
  LOG_TRACE("Compiler: MSVC {}.{} | C++ std: {}", _MSC_VER / 100, _MSC_VER % 100, _MSVC_LANG);
#endif

  try
  {
    //ENGINE_PROFILE_BEGIN("Editor initialization", "profile_init.json");
    auto app = Editor::EditorApp({ argc, argv });
    //ENGINE_PROFILE_END();

    //ENGINE_PROFILE_BEGIN("Editor runtime", "profile_runtime.json");
    app.Run();
    //ENGINE_PROFILE_END();

    //ENGINE_PROFILE_BEGIN("Editor shutdown", "profile_shutdown.json");
  }
  catch (const std::exception& e)
  {
    LOG_CRITICAL("{}", e.what());
    return EXIT_FAILURE;
  }

  //ENGINE_PROFILE_END();
}