#pragma once

#include "Engine/Scene/Entity.h"
#include <vector>
#include <string>

namespace Engine
{
  enum class ScriptFieldType
  {
    Float,
    Int,
    Bool
  };

  struct ScriptField
  {
    const char* Name;
    ScriptFieldType Type;
    size_t Offset;
  };

  struct ScriptDescriptor
  {
    const char* Name;
    size_t Size;

    void (*OnCreate)(void*, Entity);
    void (*OnDestroy)(void*);
    void (*OnUpdate)(void*, Entity, float);

    std::vector<ScriptField> Fields;
  };
}
