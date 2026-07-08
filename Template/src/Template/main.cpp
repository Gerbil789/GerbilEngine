#include "Engine/Utility/Path.h"
#include "Engine/Core/Log.h"
#include "Template/Core/TemplateApp.h"

int main()
{
  SetupWorkingDirectory();
  Engine::Log::Initialize();

  try
  {
    Template::TemplateApp app;
    app.Run();
  }
  catch (const std::exception& e)
  {
    LOG_CRITICAL("{}", e.what());
    return EXIT_FAILURE;
  }
}