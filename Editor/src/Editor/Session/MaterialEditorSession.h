#pragma once

#include "Editor/Session/EditorSession.h"
#include "Engine/Renderer/Material.h"

namespace Editor
{
  class MaterialEditorSession : public EditorSession
  {
  public:
    void SetActiveMaterial(const Ref<Engine::Material>& material) { m_ActiveMaterial = material; }
    Ref<Engine::Material> GetActiveMaterial() const { return m_ActiveMaterial; }

  private:
    Ref<Engine::Material> m_ActiveMaterial = nullptr;
  };
}