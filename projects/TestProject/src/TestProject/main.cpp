#include "Game.h"
#include "Engine/Core/Log.h"

int main()
{
  try
  {
    Game game;
  }
  catch (const std::exception& e)
  {
    LOG_CRITICAL("{}", e.what());
    return EXIT_FAILURE;
  }
}