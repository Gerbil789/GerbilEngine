#pragma once

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
      auto command = std::make_unique<T>(std::forward<Args>(args)...);
      command->Execute();
      s_UndoStack.push(std::move(command));

      // clear redo stack
      std::stack<std::unique_ptr<ICommand>> empty;
      s_RedoStack.swap(empty);
    }

    static void Undo();
    static void Redo();
    static void OnEvent(Engine::Event& e);
   
  private:
		//const static int s_MaxUndoSteps = 128; //TODO: limit undo steps

    inline static std::stack<std::unique_ptr<ICommand>> s_UndoStack;
    inline static std::stack<std::unique_ptr<ICommand>> s_RedoStack;
  };
}