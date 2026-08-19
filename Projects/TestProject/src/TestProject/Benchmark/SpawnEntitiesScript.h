#pragma once

#include "Engine/Script/Script.h" 
#include "Engine/Core/Log.h"


class SpawnEntitiesScript : public Engine::Script
{
  SCRIPT_CLASS(SpawnEntitiesScript)

public:
  PROPERTY(m_GridWidth, "grid width")
    int m_GridWidth = 10;

  PROPERTY(m_GridHeight, "grid height")
    int m_GridHeight = 10;

  PROPERTY(m_Offset, "offset")
    float m_Offset = 5.0f;

  PROPERTY(m_Mesh, "mesh")
    Engine::Mesh m_Mesh;

  PROPERTY(m_Material, "material")
    Engine::Material m_Material;

public:
  void OnStart() override;
  void OnUpdate() override;
  void OnDestroy() override;
};