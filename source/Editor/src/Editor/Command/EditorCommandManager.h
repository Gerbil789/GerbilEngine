#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Command/RemoveComponentCommand.h"
#include "Editor/Command/AddComponentCommand.h"
#include "Editor/Command/ComponentSnapshotCommand.h"
#include "Editor/Command/BatchCommand.h"
#include "Engine/Core/UUID.h"
#include "Editor/Command/CreateEntity.h"
#include "Engine/Core/Components.h"
#include <stack>
#include <vector>

namespace editor
{
  class EditorCommandManager 
  {
  public:
		static void Initialize();

    template<typename... Components>
    static void CreateEntity(const std::string& name = "Empty", entt::entity parent = entt::null)
    {
      Enqueue(std::make_unique<CreateEntityCommand<Components...>>(name, parent));
    }

    static void DeleteEntity(engine::Uuid entityId);
		static void OpenScene(engine::Scene scene);

    template<typename T>
    static void AddComponent(engine::Entity entity, const T& initial)
    {
      Enqueue(std::make_unique<AddComponentCommand<T>>(entity, initial));
    }

    template<typename T>
    static void RemoveComponent(engine::Entity entity)
    {
      Enqueue(std::make_unique<RemoveComponentCommand<T>>(entity));
    }

    template<typename T>
    static void ModifyComponent(engine::Entity entity, const T& before, const T& after)
    {
      ModifyComponents<T>({ entity }, { before }, { after });
    }

    template<typename T>
    static void ModifyComponents(const std::vector<engine::Entity>& entities, const std::vector<T>& before, const std::vector<T>& after)
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
		//const static int s_MaxUndoSteps = 128; //TODO: limit undo steps

    inline static std::stack<std::unique_ptr<ICommand>> s_UndoStack;
    inline static std::stack<std::unique_ptr<ICommand>> s_RedoStack;
    inline static std::vector<std::unique_ptr<ICommand>> s_Deferred;
  };
}