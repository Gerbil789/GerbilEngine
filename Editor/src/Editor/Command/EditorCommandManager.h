#pragma once

#include "Editor/Command/ICommand.h"
#include <stack>
#include <vector>

namespace Engine { class Event; }

namespace Editor
{
  class EditorCommandManager 
  {
  public:
		template <typename T, typename... Args>
    static void ExecuteCommand(Args&&... args)
    {
      auto command = new T(std::forward<Args>(args)...);
      command->Execute();
      s_UndoStack.push(command);

      // clear redo stack
      std::stack<ICommand*> empty;
      s_RedoStack.swap(empty);
    }

    template <typename T, typename... Args>
    static void EnqueueCommand(Args&&... args)
    {
      s_Deferred.emplace_back(new T(std::forward<Args>(args)...) );
    }

    static void Undo();
    static void Redo();
    static void OnEvent(Engine::Event& e);

    static void Flush();
   
  private:
		//const static int s_MaxUndoSteps = 128; //TODO: limit undo steps

    inline static std::stack<ICommand*> s_UndoStack;
    inline static std::stack<ICommand*> s_RedoStack;

    inline static std::vector<ICommand*> s_Deferred;
  };
}