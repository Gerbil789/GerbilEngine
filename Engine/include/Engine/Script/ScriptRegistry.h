#pragma once

#include "Engine/Core/UUID.h"
#include <string_view>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeinfo>
#include <xhash>

namespace Engine
{
  class Script;
  enum class ScriptFieldType { Float, Int, Bool, Texture, AudioClip, Mesh, Shader, Material };

  struct ENGINE_API ScriptField
  {
    std::string name;
    ScriptFieldType type;
    size_t offset;

    template<typename T>
    T& GetValue(Engine::Script* instance) const
    {
      std::byte* base = reinterpret_cast<std::byte*>(instance);
      return *reinterpret_cast<T*>(base + offset);
    }
  };

  struct ENGINE_API ScriptDescriptor
  {
		uint32_t id;
    std::string name;
    std::vector<ScriptField> fields;
    std::function<Script* ()> factory;
  };

  namespace ScriptRegistry
  {
    ENGINE_API const std::unordered_map<uint32_t, ScriptDescriptor>& GetScripts();
    ENGINE_API const ScriptDescriptor& GetDescriptor(uint32_t id);
    ENGINE_API bool HasScript(uint32_t id);

    ENGINE_API void Internal_AddScript(const ScriptDescriptor& desc);

    template<typename T>
    void Register(const char* scriptName)
    {
      ScriptDescriptor desc;
      desc.id = static_cast<uint32_t>(std::hash<std::string>{}(scriptName));
      desc.name = scriptName;
      desc.factory = [] { return new T(); };
      desc.fields = T::GetProperties();

      Internal_AddScript(desc);
    }
  };
}