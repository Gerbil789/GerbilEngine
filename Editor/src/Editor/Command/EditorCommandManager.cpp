#include "EditorCommandManager.h"
#include "Editor/Command/CreateEntity.h"
#include "Editor/Command/DeleteEntity.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Core/Input.h"

namespace Editor
{
  void EditorCommandManager::Initialize()
  {
    s_SceneChangedListener = Engine::EventBus::Subscribe<Engine::KeyPressedEvent>([](auto e)
      {
        if ((e.key == Engine::Key::Z || e.key == Engine::Key::Y) && Engine::Input::IsKeyDown(Engine::Key::LeftControl))
        {
          (!Engine::Input::IsKeyDown(Engine::Key::LeftShift) ? Undo() : Redo());
        }
        return false;
      });
  }

  void EditorCommandManager::SetContext(Engine::Scene* scene)
  { 
    s_Context = scene;

		//clear stacks
    while (!s_UndoStack.empty())
    {
      s_UndoStack.pop();
    }
    while (!s_RedoStack.empty())
    {
      s_RedoStack.pop();
    }
		s_Deferred.clear();
  }

	void EditorCommandManager::CreateEntity(const std::string& name)
  {
    Enqueue(std::make_unique<CreateEntityCommand>(s_Context, name));
  }

	void EditorCommandManager::DeleteEntity(entt::entity entity)
  {
    Enqueue(std::make_unique<DeleteEntityCommand>(s_Context, entity));
  }

  void EditorCommandManager::TransformEntity(entt::entity entity, const TransformData& before, const TransformData& after)
  {
		entt::registry& registry = s_Context->GetRegistry();
		Enqueue(std::make_unique<TransformEntityCommand>(registry, entity, before, after));
  }

  void EditorCommandManager::TransformEntities(const std::vector<entt::entity>& entities, const std::vector<TransformData>& before, const std::vector<TransformData>& after)
  {
		entt::registry& registry = s_Context->GetRegistry();
		Enqueue(std::make_unique<TransformEntitiesCommand>(registry, entities, before, after));
  }

  void EditorCommandManager::Enqueue(std::unique_ptr<ICommand> cmd)
  {
    s_Deferred.emplace_back(std::move(cmd));
  }

  void EditorCommandManager::Undo()
  {
    if (s_UndoStack.empty()) return;

    auto cmd = std::move(s_UndoStack.top());
    s_UndoStack.pop();
    cmd->Undo();
    s_RedoStack.push(std::move(cmd));
  }

  void EditorCommandManager::Redo()
  {
    if (s_RedoStack.empty()) return;

    auto cmd = std::move(s_RedoStack.top());
    s_RedoStack.pop();

    cmd->Execute();
    s_UndoStack.push(std::move(cmd));
  }

  void EditorCommandManager::ExecuteDefferedCommands()
  {
    for (auto& cmd : s_Deferred)
    {
      cmd->Execute();
      s_UndoStack.push(std::move(cmd));
    }

    s_Deferred.clear();

    // clear redo
    while (!s_RedoStack.empty())
    {
      s_RedoStack.pop();
    }

  }
}