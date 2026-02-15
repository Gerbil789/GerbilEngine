#include "EditorCommandManager.h"
#include "Editor/Command/CreateEntity.h"
#include "Editor/Command/DeleteEntity.h"

namespace Editor
{
	void EditorCommandManager::CreateEntity(const std::string& name)
  {
    Enqueue(std::make_unique<CreateEntityCommand>(name));
  }

	void EditorCommandManager::DeleteEntity(Engine::Entity entity)
  {
    Enqueue(std::make_unique<DeleteEntityCommand>(entity));
  }

  void EditorCommandManager::Enqueue(std::unique_ptr<ICommand> cmd)
  {
    s_Deferred.emplace_back(std::move(cmd));
  }

  void EditorCommandManager::Undo()
  {
    if (s_UndoStack.empty()) return;

    auto cmd = std::move(s_UndoStack.top());
    s_UndoStack.pop();
    cmd->Undo();
    s_RedoStack.push(std::move(cmd));
  }

  void EditorCommandManager::Redo()
  {
    if (s_RedoStack.empty()) return;

    auto cmd = std::move(s_RedoStack.top());
    s_RedoStack.pop();

    cmd->Execute();
    s_UndoStack.push(std::move(cmd));
  }

  void EditorCommandManager::Flush()
  {
    for (auto& cmd : s_Deferred)
    {
      cmd->Execute();
      s_UndoStack.push(std::move(cmd));
    }

    s_Deferred.clear();

    // clear redo
    while (!s_RedoStack.empty())
    {
      s_RedoStack.pop();
    }

  }
}