#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Core/GameContext.h"

#ifdef ENGINE_PLATFORM_WINDOWS
#ifdef GAME_BUILD_DLL
#define GAME_API  __declspec(dllexport)
#else
#define GAME_API  __declspec(dllimport)     
#endif
#else
#define GAME_API
#endif

namespace Engine
{
  class Script;
  enum class ScriptFieldType { Float, Int, Bool, Texture, AudioClip, Mesh, Material };

  struct ScriptField
  {
    std::string name;
    ScriptFieldType type;
    size_t offset;
  };

  struct ScriptDescriptor
  {
    std::string name;
    std::vector<ScriptField> fields;
    std::function<Script* ()> factory;
  };

  class ScriptRegistry
  {
  public:
    ScriptRegistry() = default;
    ScriptRegistry(const ScriptRegistry&) = delete;
    ScriptRegistry& operator=(const ScriptRegistry&) = delete;

    inline std::string ScriptName(std::string name)
    {
      constexpr std::string_view struct_kw = "struct ";
      constexpr std::string_view class_kw = "class ";

      if (name.starts_with(struct_kw))
        name.erase(0, struct_kw.size());
      else if (name.starts_with(class_kw))
        name.erase(0, class_kw.size());

      return name;
    }

    template<typename T>
    void Register()
    {
      auto name = ScriptName(typeid(T).name());

      ScriptDescriptor desc;
      desc.name = name;
      desc.factory = [] { return new T(); };
      desc.fields = T::GetProperties();
      m_Registry.emplace(name, std::move(desc));
    }

    inline ScriptDescriptor& GetDescriptor(std::string id) { return m_Registry.at(id); }

    inline std::vector<const ScriptDescriptor*> GetAllDescriptors() const
    {
      std::vector<const ScriptDescriptor*> result;
      result.reserve(m_Registry.size());

      for (auto& [_, desc] : m_Registry)
        result.push_back(&desc);

      return result;
    }

    inline std::vector<std::string> GetAllScriptNames()
    {
      std::vector<std::string> scriptNames; //TODO: cache this, this is getting called every frame
      scriptNames.reserve(m_Registry.size());
      for (const auto& [_, desc] : m_Registry)
        scriptNames.push_back(desc.name);
      return scriptNames;
    }

    inline void Clear()
    {
      m_Registry.clear();
    }

  private:
		std::unordered_map<std::string, ScriptDescriptor> m_Registry;
  };


  extern ENGINE_API ScriptRegistry g_ScriptRegistry;
}


extern "C"
{
  GAME_API void RegisterScripts(Engine::ScriptRegistry& registry);
}