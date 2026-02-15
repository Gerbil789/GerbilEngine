#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Command/RemoveComponentCommand.h"
#include "Editor/Command/AddComponentCommand.h"
#include "Editor/Command/ComponentSnapshotCommand.h"
#include <stack>
#include <vector>

namespace Editor
{
  class EditorCommandManager 
  {
  public:
    static void CreateEntity(const std::string& name = "Empty");
    static void DeleteEntity(Engine::Entity entity);

    template<typename T>
    static void RemoveComponent(Engine::Entity e)
    {
      Enqueue(std::make_unique<RemoveComponentCommand<T>>(e));
    }

    template<typename T>
    static void AddComponent(Engine::Entity e, const T& initial)
    {
      Enqueue(std::make_unique<AddComponentCommand<T>>(e, initial));
    }

    template<typename T>
    static void ModifyComponent(Engine::Entity e, const T& before, const T& after)
    {
      Enqueue(std::make_unique<ComponentSnapshotCommand<T>>(e, before, after));
    }

    static void Enqueue(std::unique_ptr<ICommand> cmd);
    static void Undo();
    static void Redo();
    static void Flush();
   
  private:
		//const static int s_MaxUndoSteps = 128; //TODO: limit undo steps

    inline static std::stack<std::unique_ptr<ICommand>> s_UndoStack;
    inline static std::stack<std::unique_ptr<ICommand>> s_RedoStack;
    inline static std::vector<std::unique_ptr<ICommand>> s_Deferred;
  };
}