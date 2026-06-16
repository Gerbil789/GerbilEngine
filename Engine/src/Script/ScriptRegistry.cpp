#include "enginepch.h"
#include "Engine/Script/ScriptRegistry.h"

namespace Engine::ScriptRegistry
{
  static std::unordered_map<uint32_t, ScriptDescriptor> m_Scripts;

  const std::unordered_map<uint32_t, ScriptDescriptor>& GetScripts()
  {
    return m_Scripts;
  }

  const ScriptDescriptor& GetDescriptor(uint32_t id)
  {
    return m_Scripts.at(id);
  }

  bool HasScript(uint32_t id)
  {
    return m_Scripts.contains(id);
  }

  void Internal_AddScript(const ScriptDescriptor& desc)
  {
    m_Scripts[desc.id] = desc;
  }
}