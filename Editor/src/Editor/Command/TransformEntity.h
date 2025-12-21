#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

namespace Editor
{
  struct TransformData
  {
    glm::vec3 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale;
	};


  class TransformEntityCommand : public ICommand
  {
  public:
    TransformEntityCommand(Engine::Entity entity, const TransformData& before, const TransformData& after)
      : m_Entity(entity), m_Before(before), m_After(after) {}

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
      auto& tc = m_Entity.GetComponent<Engine::TransformComponent>();
      tc.Position = data.Position;
      tc.Rotation = data.Rotation;
      tc.Scale = data.Scale;
    }

  private:
    Engine::Entity m_Entity;
    TransformData m_Before, m_After;
  };


  class TransformEntitiesCommand : public ICommand
  {
  public:
    TransformEntitiesCommand(std::vector<Engine::Entity> entities, const std::vector<TransformData>& before, const std::vector<TransformData>& after)
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
      for (size_t i = 0; i < m_Entities.size(); i++)
      {
        auto& tc = m_Entities[i].GetComponent<Engine::TransformComponent>();
        tc.Position = data[i].Position;
        tc.Rotation = data[i].Rotation;
        tc.Scale = data[i].Scale;
      }
    }

  private:
    std::vector<Engine::Entity> m_Entities;
    std::vector<TransformData> m_Before, m_After;
  };

}

