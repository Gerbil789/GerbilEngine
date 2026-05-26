#pragma once

#include "Editor/Command/ICommand.h"
#include "Editor/Command/RemoveComponentCommand.h"
#include "Editor/Command/AddComponentCommand.h"
#include "Editor/Command/ComponentSnapshotCommand.h"
#include "Editor/Command/TransformEntity.h"
#include "Engine/Scene/Scene.h"
#include <stack>
#include <vector>

namespace Editor
{
  class EditorCommandManager 
  {
  public:
		static void Initialize();
    static void SetContext(Engine::Scene* scene);

    static void CreateEntity(const std::string& name = "Empty");
    static void DeleteEntity(entt::entity entity);
    static void TransformEntity(entt::entity entity, const TransformData& before, const TransformData& after);
    static void TransformEntities(const std::vector<entt::entity>& entities, const std::vector<TransformData>& before, const std::vector<TransformData>& after);

    template<typename T>
    static void AddComponent(entt::entity e, const T& initial)
    {
      entt::registry& registry = s_Context->GetRegistry();
      Enqueue(std::make_unique<AddComponentCommand<T>>(registry, e, initial));
    }

    template<typename T>
    static void RemoveComponent(entt::entity e)
    {
      entt::registry& registry = s_Context->GetRegistry();
      Enqueue(std::make_unique<RemoveComponentCommand<T>>(registry, e));
    }

    template<typename T>
    static void ModifyComponent(entt::entity e, const T& before, const T& after)
    {
      entt::registry& registry = s_Context->GetRegistry();
      Enqueue(std::make_unique<ComponentSnapshotCommand<T>>(registry, e, before, after));
    }

    static void Enqueue(std::unique_ptr<ICommand> cmd);
    static void Undo();
    static void Redo();
    static void Flush();
   
  private:
		//const static int s_MaxUndoSteps = 128; //TODO: limit undo steps
		inline static Engine::Scene* s_Context = nullptr;

    inline static std::stack<std::unique_ptr<ICommand>> s_UndoStack;
    inline static std::stack<std::unique_ptr<ICommand>> s_RedoStack;
    inline static std::vector<std::unique_ptr<ICommand>> s_Deferred;
  };
}