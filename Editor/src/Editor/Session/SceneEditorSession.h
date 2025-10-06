#pragma once

#include "Editor/Session/EditorSession.h"
#include "Engine/Scene/Entity.h"
#include "Editor/Command/SceneCommands.h"

namespace Editor
{
  class SceneEditorSession : public EditorSession
  {
  public:
    void SelectEntity(Engine::Entity entity, bool additive = false)
    {
      if (!additive)
        m_SelectedEntities.clear();

      if (!IsEntitySelected(entity))
      {
        m_SelectedEntities.push_back(entity);
      }

			LOG_TRACE("Selected entities: {0}", m_SelectedEntities.size());
    }

    void DeselectEntity(Engine::Entity entity)
    {
      m_SelectedEntities.erase(
        std::remove(m_SelectedEntities.begin(), m_SelectedEntities.end(), entity),
        m_SelectedEntities.end()
      );
    }

    void ClearSelection()
    {
      m_SelectedEntities.clear();
    }

    const std::vector<Engine::Entity>& GetSelectedEntities() const
    {
      return m_SelectedEntities;
    }

    bool IsEntitySelected(Engine::Entity entity) const
    {
      return std::find(m_SelectedEntities.begin(), m_SelectedEntities.end(), entity) != m_SelectedEntities.end();
    }

    Engine::Entity GetActiveEntity() const
    {
      return m_SelectedEntities.empty() ? Engine::Entity::Null() : m_SelectedEntities.front();
    }

    Engine::Entity CreateEntity(const std::string& name = "Empty Entity")
    {
      auto command = CreateScope<Editor::CreateEntityCommand>(name);
      auto* raw = command.get();
      m_CommandManager.ExecuteCommand(std::move(command));

			// select the newly created entity
			Engine::Entity newEntity = raw->GetEntity();
			SelectEntity(newEntity);
			return newEntity;
		}

  protected:
    void OnSceneChanged() override
    {
      ClearSelection();
    }

  private:
		std::vector<Engine::Entity> m_SelectedEntities;
  };
}