#include "EditorCommandManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/KeyEvent.h"
#include <filesystem>

namespace Editor
{
	void EditorCommandManager::Undo()
  {
    if (s_UndoStack.empty())
    {
      return;
    }

    auto command = s_UndoStack.top();
    s_UndoStack.pop();
    command->Undo();
    s_RedoStack.push(command);
  }

  void EditorCommandManager::Redo()
  {
    if (s_RedoStack.empty())
    {
      return;
    }

    auto command = s_RedoStack.top();
    s_RedoStack.pop();
    command->Execute();
    s_UndoStack.push(command);
  }

  void EditorCommandManager::OnEvent(Engine::Event& e)
  {
    if (e.GetEventType() == Engine::EventType::KeyPressed)
    {
      auto& event = static_cast<Engine::KeyPressedEvent&>(e);
      if ((event.GetKey() == Engine::Key::Z || event.GetKey() == Engine::Key::Y) && Engine::Input::IsKeyPressed(Engine::Key::LeftControl))
      {
        if (!Engine::Input::IsKeyPressed(Engine::Key::LeftShift))
        {
          Undo();
        }
        else
        {
          Redo();
        }
      }
    }
  }

  void EditorCommandManager::Flush()
  {
    if (s_Deferred.empty()) return;

    for (ICommand* command : s_Deferred)
    {
      command->Execute();
      s_UndoStack.push(command);
    }

    s_Deferred.clear();

    // structural changes invalidate redo history
    std::stack<ICommand*> empty;
    s_RedoStack.swap(empty);
  }
}