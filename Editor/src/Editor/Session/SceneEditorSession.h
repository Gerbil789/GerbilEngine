#pragma once

#include "Editor/Session/EditorSession.h"
#include "Engine/Scene/Entity.h"
#include "Editor/Command/SceneCommands.h"

namespace Editor
{
  class SceneEditorSession : public EditorSession
  {
  public:
    void SelectEntity(Engine::Entity entity) { m_SelectedEntity = entity; }
    void DeselectEntity() { m_SelectedEntity = entt::null; }

    Engine::Entity GetSelectedEntity() const { return { m_SelectedEntity, m_Scene }; }
    bool IsEntitySelected(Engine::Entity entity) const { return m_SelectedEntity == entity; }

    Engine::Entity CreateEntity(const std::string& name = "Empty Entity")
    {
      auto command = CreateScope<Editor::CreateEntityCommand>(name);
      auto* raw = command.get();
      m_CommandManager.ExecuteCommand(std::move(command));

			// select the newly created entity
      Engine::Entity newEntity = raw->GetEntity();
			m_SelectedEntity = newEntity;
			return newEntity;
		}

  protected:
    void OnSceneChanged() override
    {
      m_SelectedEntity = entt::null;
    }

  private:
    entt::entity m_SelectedEntity = entt::null;
  };
}