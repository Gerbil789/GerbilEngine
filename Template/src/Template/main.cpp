#include "Engine/Core/Log.h"
#include "Template/Core/TemplateApp.h"

int main(int argc, char** argv)
{
  Engine::Log::Initialize();

  try
  {
    auto app = Template::TemplateApp({ argc, argv });
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