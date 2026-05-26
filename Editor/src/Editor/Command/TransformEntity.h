#pragma once

#include "Editor/Command/ICommand.h"
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
    TransformEntityCommand(entt::registry& registry, entt::entity entity, const TransformData& before, const TransformData& after) : m_Registry(&registry), m_Entity(entity), m_Before(before), m_After(after) {}

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
			auto& tc = m_Registry->get<Engine::TransformComponent>(m_Entity);
      tc.position = data.Position;
      tc.rotation = data.Rotation;
      tc.scale = data.Scale;
    }

  private:
    entt::registry* m_Registry;
    entt::entity m_Entity;
    TransformData m_Before, m_After;
  };


  class TransformEntitiesCommand : public ICommand
  {
  public:
    TransformEntitiesCommand(entt::registry& registry, std::vector<entt::entity> entities, const std::vector<TransformData>& before, const std::vector<TransformData>& after)
      : m_Registry(&registry), m_Entities(entities), m_Before(before), m_After(after) {}

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
      for (size_t i = 0; i < m_Entities.size(); i++)
      {
        auto& tc = m_Registry->get<Engine::TransformComponent>(m_Entities[i]);
        tc.position = data[i].Position;
        tc.rotation = data[i].Rotation;
        tc.scale = data[i].Scale;
      }
    }

  private:
		entt::registry* m_Registry;
    std::vector<entt::entity> m_Entities;
    std::vector<TransformData> m_Before, m_After;
  };

}

