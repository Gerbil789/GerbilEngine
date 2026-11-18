#include "editor/core/editor_command_manager.h"

namespace editor
{
  void EditorCommandManager::Enqueue(std::function<void()> action)
  {
    s_deferredActions.push_back(std::move(action));
  }

  void EditorCommandManager::ExecuteDeferred()
  {
    if (s_deferredActions.empty()) return;

    std::vector<std::function<void()>> actionsToExecute = std::move(s_deferredActions);

    for (auto& action : actionsToExecute)
    {
      action();
    }
  }
}