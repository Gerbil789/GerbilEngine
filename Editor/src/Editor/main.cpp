#include "enginepch.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"
#include "Editor/Core/EditorApp.h"

#ifdef ENGINE_PLATFORM_WINDOWS

int main(int argc, char** argv)
{
  Engine::Log::Init();

  ENGINE_PROFILE_BEGIN_SESSION("Startup", "GerbilProfile-Startup.json");
  auto app = new Editor::EditorApp();
  ENGINE_PROFILE_END_SESSION();

  ENGINE_PROFILE_BEGIN_SESSION("Runtime", "GerbilProfile-Runtime.json");
  app->Run();
  ENGINE_PROFILE_END_SESSION();

  ENGINE_PROFILE_BEGIN_SESSION("Shutdown", "GerbilProfile-Shutdown.json");
  delete app;
  ENGINE_PROFILE_END_SESSION();
}

#else
#error Engine only supports Windows!
#endif