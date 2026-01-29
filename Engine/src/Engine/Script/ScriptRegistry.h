#pragma once

#include "Engine/Script/ScriptDescriptor.h"
#include "Engine/Core/UUID.h"

namespace Engine
{
  struct RegisteredScript
  {
    UUID ID;
    ScriptDescriptor Desc;
  };

  class ScriptRegistry
  {
  public:
    static UUID Register(const ScriptDescriptor& desc);
    static const RegisteredScript* Get(UUID id);
    static const RegisteredScript* GetByName(const char* name);
    static const std::vector<RegisteredScript>& GetAll();
  };
}
