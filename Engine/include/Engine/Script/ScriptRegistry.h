#pragma once

#include "Engine/Script/ScriptDescriptor.h"
#include "Engine/Core/UUID.h"


#include "Engine/Core/GameContext.h"
//#include "Engine/Script/ScriptRegistry.h"

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
  class ScriptRegistry
  {
  public:
    static ScriptRegistry& Get()
    {
      static ScriptRegistry instance;
      return instance;
    }

    ScriptRegistry(const ScriptRegistry&) = delete;
    ScriptRegistry& operator=(const ScriptRegistry&) = delete;


    template<typename T>
    void Register(const std::string& name, std::vector<ScriptField> fields)
    {
      ScriptDescriptor desc;
      desc.name = name;
      desc.factory = [] { return new T(); };
      desc.fields = std::move(fields);
      m_Registry.emplace(name, std::move(desc));
      //m_Registry[name] = std::move(desc);
    }

    ScriptDescriptor& Get(std::string id) { return m_Registry.at(id); }

    std::vector<const ScriptDescriptor*> GetAll() const
    {
      std::vector<const ScriptDescriptor*> result;
      result.reserve(m_Registry.size());

      for (auto& [_, desc] : m_Registry)
        result.push_back(&desc);

      return result;
    }

    void Clear()
    {
      m_Registry.clear();
    }



  private:
    ScriptRegistry() = default;
		std::unordered_map<std::string, ScriptDescriptor> m_Registry;
  };
}


extern "C"
{
  GAME_API void RegisterScripts(Engine::ScriptRegistry& registry);

  //GAME_API void OnLoad(GameContext* context);
  //GAME_API void OnUnload();
  //GAME_API void OnStart();
  //GAME_API void OnUpdate(float delta);
}
