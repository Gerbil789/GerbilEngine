#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Command/RemoveComponentCommand.h"
#include "Editor/Command/AddComponentCommand.h"
#include "Editor/Command/ComponentSnapshotCommand.h"
#include "Editor/Command/BatchCommand.h"
#include "Engine/Core/UUID.h"
#include "Engine/Core/Components.h"
#include <stack>
#include <vector>

namespace editor
{
  class EditorCommandManager 
  {
  public:
    static void Update();

    template<typename T>
    static void AddComponent(entt::entity entity, const T& initial)
    {
      Enqueue(std::make_unique<AddComponentCommand<T>>(entity, initial));
    }

    template<typename T>
    static void RemoveComponent(entt::entity entity)
    {
      Enqueue(std::make_unique<RemoveComponentCommand<T>>(entity));
    }

    template<typename T>
    static void ModifyComponent(entt::entity entity, const T& before, const T& after)
    {
      ModifyComponents<T>({ entity }, { before }, { after });
    }

    template<typename T>
    static void ModifyComponents(const std::vector<entt::entity>& entities, const std::vector<T>& before, const std::vector<T>& after)
    {
      std::vector<std::unique_ptr<ICommand>> commands;
			commands.reserve(entities.size());

      for (size_t i = 0; i < entities.size(); ++i)
      {
        commands.push_back(std::make_unique<ComponentSnapshotCommand<T>>(entities[i], before[i], after[i]));
      }

      Enqueue(std::make_unique<BatchCommand>(std::move(commands)));
    }

    static void Enqueue(std::unique_ptr<ICommand> cmd);
    static void Undo();
    static void Redo();
    static void ExecuteDeferredCommands();
		static void Clear();
   
  private:
		//const static int s_maxUndoSteps = 128; //TODO: limit undo steps

    inline static std::stack<std::unique_ptr<ICommand>> s_undoStack;
    inline static std::stack<std::unique_ptr<ICommand>> s_redoStack;
    inline static std::vector<std::unique_ptr<ICommand>> s_deferred;
  };
}