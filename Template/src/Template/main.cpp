#include "Engine/Core/Log.h"
#include "Template/Core/TemplateApp.h"

int main()
{
  Engine::Log::Initialize();

  try
  {
    Template::TemplateApp app;
    LOG_INFO("--- Project initialization complete ---");
    app.Run();

    LOG_TRACE("--- Exiting application ---");
  }
  catch (const std::exception& e)
  {
    LOG_CRITICAL("{}", e.what());
    return EXIT_FAILURE;
  }
}