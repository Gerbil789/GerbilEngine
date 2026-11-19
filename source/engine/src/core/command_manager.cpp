#include "engine/core/command_manager.h"

namespace engine
{
  void CommandManager::Enqueue(std::function<void()> action)
  {
    s_deferredActions.push_back(std::move(action));
  }

  void CommandManager::ExecuteDeferred()
  {
    if (s_deferredActions.empty()) return;

    std::vector<std::function<void()>> actionsToExecute = std::move(s_deferredActions);

    for (auto& action : actionsToExecute)
    {
      action();
    }
  }
}