#pragma once

#include <vector>
#include <functional>

namespace editor
{
  struct EditorCommandManager
  {
    static void Enqueue(std::function<void()> action);
    static void ExecuteDeferred();
   
  private:
    inline static std::vector<std::function<void()>> s_deferredActions;
  };
}