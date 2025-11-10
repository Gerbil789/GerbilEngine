#pragma once

#include "Engine/Scene/Entity.h"

namespace Editor
{
  class EditorContext
  {
  public:
    static void SelectEntity(Engine::Entity entity, bool additive = false)
    {
      if (!additive)
        m_SelectedEntities.clear();

      if (!IsEntitySelected(entity))
      {
        m_SelectedEntities.push_back(entity);
      }

			LOG_TRACE("Selected entities: {0}", m_SelectedEntities.size());
    }

    static void DeselectEntity(Engine::Entity entity)
    {
      m_SelectedEntities.erase(
        std::remove(m_SelectedEntities.begin(), m_SelectedEntities.end(), entity),
        m_SelectedEntities.end()
      );
    }

    static void ClearSelection()
    {
      m_SelectedEntities.clear();
    }

    static const std::vector<Engine::Entity>& GetSelectedEntities()
    {
      return m_SelectedEntities;
    }

    static bool IsEntitySelected(Engine::Entity entity)
    {
      return std::find(m_SelectedEntities.begin(), m_SelectedEntities.end(), entity) != m_SelectedEntities.end();
    }

    static Engine::Entity GetActiveEntity()
    {
      return m_SelectedEntities.empty() ? Engine::Entity::Null() : m_SelectedEntities.front();
    }

  //  static Engine::Entity CreateEntity(const std::string& name = "Empty Entity")
  //  {
  //    auto command = CreateScope<Editor::CreateEntityCommand>(name);
  //    auto* raw = command.get();
  //    CommandManager::ExecuteCommand(std::move(command));

		//	// select the newly created entity
		//	Engine::Entity newEntity = raw->GetEntity();
		//	SelectEntity(newEntity);
		//	return newEntity;
		//}


  private:
    inline static std::vector<Engine::Entity> m_SelectedEntities;
  };
}