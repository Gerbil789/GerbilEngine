#include "EditorCommandManager.h"
#include "Editor/Command/DeleteEntity.h"
#include "Editor/Command/OpenSceneCommand.h"
#include "Engine/Core/Components.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Core/Input.h"

namespace Editor
{
  void EditorCommandManager::Initialize()
  {
    Engine::EventBus::Subscribe<Engine::KeyPressedEvent>([](auto e)
      {
        if ((e.key == Engine::Key::Z || e.key == Engine::Key::Y) && Engine::Input::IsKeyDown(Engine::Key::LeftControl))
        {
          (!Engine::Input::IsKeyDown(Engine::Key::LeftShift) ? Undo() : Redo());
        }
        return false;
      });
  }

	void EditorCommandManager::DeleteEntity(Engine::Uuid entityId)
  {
    Enqueue(std::make_unique<DeleteEntityCommand>(entityId));
  }

  void EditorCommandManager::OpenScene(Engine::Uuid sceneId)
  {
		Enqueue(std::make_unique<OpenSceneCommand>(sceneId));
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