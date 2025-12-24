#include "CommandManager.h"
#include "Engine/Core/Input.h"
#include <filesystem>

namespace Editor
{
	void CommandManager::Undo()
  {
    if (s_UndoStack.empty())
    {
      return;
    }

    auto command = std::move(s_UndoStack.top());
    s_UndoStack.pop();
    command->Undo();
    s_RedoStack.push(std::move(command));
  }

  void CommandManager::Redo()
  {
    if (s_RedoStack.empty())
    {
      return;
    }

    auto command = std::move(s_RedoStack.top());
    s_RedoStack.pop();
    command->Execute();
    s_UndoStack.push(std::move(command));
  }

  void CommandManager::OnEvent(Engine::Event& e)
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
}