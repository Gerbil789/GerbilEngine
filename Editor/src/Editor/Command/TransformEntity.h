#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Components.h"

namespace Editor
{
  struct TransformData
  {
		glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };
	};


  class TransformEntityCommand : public ICommand
  {
  public:
    TransformEntityCommand(entt::entity entity, const TransformData& before, const TransformData& after) : m_Entity(entity), m_Before(before), m_After(after) {}

    void Execute() override
    {
      Apply(m_After);
    }

    void Undo() override
    {
      Apply(m_Before);
    }

  private:
    void Apply(const TransformData& data)
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

			auto& tc = registry.get<Engine::TransformComponent>(m_Entity);
      tc.position = data.Position;
      tc.rotation = data.Rotation;
      tc.scale = data.Scale;
      registry.patch<Engine::TransformComponent>(m_Entity);
    }

  private:
    entt::entity m_Entity;
    TransformData m_Before, m_After;
  };


  class TransformEntitiesCommand : public ICommand
  {
  public:
    TransformEntitiesCommand(std::vector<entt::entity> entities, const std::vector<TransformData>& before, const std::vector<TransformData>& after)
      : m_Entities(entities), m_Before(before), m_After(after) {}

    void Execute() override
    {
      Apply(m_After);
    }

    void Undo() override
    {
      Apply(m_Before);
    }

  private:
    void Apply(const std::vector<TransformData>& data)
    {
			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			entt::registry& registry = scene.GetRegistry();

      for (size_t i = 0; i < m_Entities.size(); i++)
      {
        auto& tc = registry.get<Engine::TransformComponent>(m_Entities[i]);
        tc.position = data[i].Position;
        tc.rotation = data[i].Rotation;
        tc.scale = data[i].Scale;
      }
    }

  private:
    std::vector<entt::entity> m_Entities;
    std::vector<TransformData> m_Before, m_After;
  };

}

