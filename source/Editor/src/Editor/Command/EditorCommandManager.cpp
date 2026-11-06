#include "EditorCommandManager.h"
#include "Editor/Command/DeleteEntity.h"
#include "Editor/Command/OpenSceneCommand.h"
#include "Engine/Core/Components.h"
#include "Engine/Core/Input.h"

namespace editor
{
  void EditorCommandManager::Update()
  {
    if (engine::input::IsKeyReleased(engine::Key::Z) && engine::input::IsKeyDown(engine::Key::LeftControl))
    {
      (!engine::input::IsKeyDown(engine::Key::LeftShift) ? Undo() : Redo());
    }
  }

  void EditorCommandManager::DeleteEntity(entt::entity entity)
  {
    Enqueue(std::make_unique<DeleteEntityCommand>(entity));
  }

  void EditorCommandManager::OpenScene(engine::SceneHandle scene)
  {
    Enqueue(std::make_unique<OpenSceneCommand>(scene));
  }

  void EditorCommandManager::Enqueue(std::unique_ptr<ICommand> cmd)
  {
    s_deferred.emplace_back(std::move(cmd));
  }

  void EditorCommandManager::Undo()
  {
    if (s_undoStack.empty()) return;

    auto cmd = std::move(s_undoStack.top());
    s_undoStack.pop();
    cmd->Undo();
    s_redoStack.push(std::move(cmd));
  }

  void EditorCommandManager::Redo()
  {
    if (s_redoStack.empty()) return;

    auto cmd = std::move(s_redoStack.top());
    s_redoStack.pop();

    cmd->Execute();
    s_undoStack.push(std::move(cmd));
  }

  void EditorCommandManager::ExecuteDeferredCommands()
  {
    for (auto& cmd : s_deferred)
    {
      cmd->Execute();
      s_undoStack.push(std::move(cmd));
    }

    s_deferred.clear();

    // clear redo
    while (!s_redoStack.empty())
    {
      s_redoStack.pop();
    }
  }

  void EditorCommandManager::Clear()
  {
    while (!s_undoStack.empty())
    {
      s_undoStack.pop();
    }
    while (!s_redoStack.empty())
    {
      s_redoStack.pop();
    }
    s_deferred.clear();
  }
}