#include "Engine/Core/Log.h"
#include "Template/Core/TemplateApp.h"

int main()
{
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