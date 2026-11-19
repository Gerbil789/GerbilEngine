#pragma once

#include <vector>
#include <functional>

namespace engine
{
  struct CommandManager
  {
    static void Enqueue(std::function<void()> action);
    static void ExecuteDeferred();
   
  private:
    inline static std::vector<std::function<void()>> s_deferredActions;
  };
}