#pragma once

#include "Engine/Core/Core.h"
#include "Editor/Command/ICommand.h"
#include <stack>

namespace Editor
{
  class CommandManager 
  {
  public:
    void ExecuteCommand(Scope<ICommand> command) 
    {
      command->Execute();
      m_UndoStack.push(std::move(command));
      while (!m_RedoStack.empty()) m_RedoStack.pop(); // clear redo
    }

    void Undo() 
    {
      if (m_UndoStack.empty())
      {
				LOG_TRACE("Undo stack is empty, cannot undo.");
        return;
      }
        
      auto command = std::move(m_UndoStack.top());
      m_UndoStack.pop();
      command->Undo();
      m_RedoStack.push(std::move(command));
    }

    void Redo() 
    {
      if (m_RedoStack.empty())
      {
        LOG_TRACE("Redo stack is empty, cannot redo.");
				return;
      }

      auto command = std::move(m_RedoStack.top());
      m_RedoStack.pop();
      command->Execute();
      m_UndoStack.push(std::move(command));
    }

  private:
    std::stack<Scope<ICommand>> m_UndoStack;
    std::stack<Scope<ICommand>> m_RedoStack;
  };
}