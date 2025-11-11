#pragma once

#include "Editor/Command/ICommand.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

namespace Editor
{
  class TransformEntityCommand : public ICommand
  {
  public:
    TransformEntityCommand(Engine::Entity entity, const glm::vec3& beforePos, const glm::vec3& beforeRot, const glm::vec3& beforeScale, const glm::vec3& afterPos, const glm::vec3& afterRot, const glm::vec3& afterScale)
      : m_Entity(entity), m_BeforePos(beforePos), m_BeforeRot(beforeRot), m_BeforeScale(beforeScale), m_AfterPos(afterPos), m_AfterRot(afterRot), m_AfterScale(afterScale) {}

    void Execute() override
    {
      Apply(m_AfterPos, m_AfterRot, m_AfterScale);
    }

    void Undo() override
    {
      Apply(m_BeforePos, m_BeforeRot, m_BeforeScale);
    }

  private:
    void Apply(const glm::vec3& p, const glm::vec3& r, const glm::vec3& s)
    {
      auto& tc = m_Entity.GetComponent<Engine::TransformComponent>();
      tc.Position = p;
      tc.Rotation = r;
      tc.Scale = s;
    }

  private:
    Engine::Entity m_Entity;
    glm::vec3 m_BeforePos, m_BeforeRot, m_BeforeScale;
    glm::vec3 m_AfterPos, m_AfterRot, m_AfterScale;
  };

}

