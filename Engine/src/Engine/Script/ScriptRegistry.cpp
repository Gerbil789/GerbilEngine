#include "enginepch.h"
#include "ScriptRegistry.h"

namespace Engine
{
  static std::vector<RegisteredScript> s_Scripts;

  UUID ScriptRegistry::Register(const ScriptDescriptor& desc)
  {
    UUID id = UUID();
    s_Scripts.push_back({ id, desc });
    return id;
  }

  const RegisteredScript* ScriptRegistry::Get(UUID id)
  {
    for (const auto& script : s_Scripts)
    {
      if (script.ID == id)
      {
        return &script;
      }
    }
		return nullptr;

  }

  const RegisteredScript* ScriptRegistry::GetByName(const char* name)
  {
    for (const auto& script : s_Scripts)
    {
      if (strcmp(script.Desc.Name, name) == 0)
      {
        return &script;
      }
    }
		return nullptr;

  }

  const std::vector<RegisteredScript>& ScriptRegistry::GetAll()
  {
    return s_Scripts;
  }

}