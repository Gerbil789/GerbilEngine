#pragma once

#include "Engine/Core/Core.h"
#include "Editor/Command/ICommand.h"
#include "Engine/Event/KeyEvent.h"
#include <stack>

namespace Editor
{
  class CommandManager 
  {
  public:
		template <typename T, typename... Args>
    static void ExecuteCommand(Args&&... args)
    {
      auto command = CreateScope<T>(std::forward<Args>(args)...);
      command->Execute();
      s_UndoStack.push(std::move(command));

      // clear redo stack
      std::stack<Scope<ICommand>> empty;
      s_RedoStack.swap(empty);
    }

    static void Undo()
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

    static void Redo()
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

    static void OnEvent(Engine::Event& e)
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

  private:
		//const static int s_MaxUndoSteps = 128; //TODO: limit undo steps

    inline static std::stack<Scope<ICommand>> s_UndoStack;
    inline static std::stack<Scope<ICommand>> s_RedoStack;
  };
}