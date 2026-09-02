#include "EditorCommandManager.h"
#include "Editor/Command/DeleteEntity.h"
#include "Editor/Command/OpenSceneCommand.h"
#include "Engine/Core/Components.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Core/Input.h"

namespace editor
{
  void EditorCommandManager::Initialize()
  {
    engine::EventBus::Subscribe<engine::KeyPressedEvent>([](auto e)
      {
        if ((e.key == engine::Key::Z || e.key == engine::Key::Y) && engine::Input::IsKeyDown(engine::Key::LeftControl))
        {
          (!engine::Input::IsKeyDown(engine::Key::LeftShift) ? Undo() : Redo());
        }
        return false;
      });
  }

	void EditorCommandManager::DeleteEntity(engine::Uuid entityId)
  {
    Enqueue(std::make_unique<DeleteEntityCommand>(entityId));
  }

  void EditorCommandManager::OpenScene(engine::Scene scene)
  {
		Enqueue(std::make_unique<OpenSceneCommand>(scene));
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

  void EditorCommandManager::ExecuteDeferredCommands()
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

  void EditorCommandManager::Clear()
  {
    while (!s_UndoStack.empty())
    {
      s_UndoStack.pop();
    }
    while (!s_RedoStack.empty())
    {
      s_RedoStack.pop();
    }
    s_Deferred.clear();
	}
}