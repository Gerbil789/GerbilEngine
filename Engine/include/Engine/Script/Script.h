#pragma once

#include "Engine/Scene/Entity.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Audio/AudioClip.h"

namespace Engine 
{
  template<typename T>
  struct ScriptFieldTypeMap;

  template<> struct ScriptFieldTypeMap<float> {
    static constexpr ScriptFieldType value = ScriptFieldType::Float;
  };

  template<> struct ScriptFieldTypeMap<int> {
    static constexpr ScriptFieldType value = ScriptFieldType::Int;
  };

  template<> struct ScriptFieldTypeMap<bool> {
    static constexpr ScriptFieldType value = ScriptFieldType::Bool;
  };

  template<> struct ScriptFieldTypeMap<Texture2D*> {
    static constexpr ScriptFieldType value = ScriptFieldType::Texture;
  };

  template<> struct ScriptFieldTypeMap<AudioClip*> {
    static constexpr ScriptFieldType value = ScriptFieldType::AudioClip;
	};

}

namespace Engine
{
  class Script
  {
  public:
    virtual ~Script() = default;

    Entity Self;

    virtual void OnCreate() {}
    virtual void OnStart() {}
    virtual void OnUpdate() {}
    virtual void OnDestroy() {}
  };
}

#define SCRIPT_CLASS(Type) \
public: \
    using __ScriptOwner = Type; \
    static std::vector<Engine::ScriptField>& GetProperties() { \
        static std::vector<Engine::ScriptField> props; \
        return props; \
    }

#define PROPERTY(Member, DisplayName) \
private: \
    struct _AutoRegister_##Member { \
        _AutoRegister_##Member() { \
            using Owner = __ScriptOwner; \
            Owner::GetProperties().push_back({ \
                DisplayName, \
                Engine::ScriptFieldTypeMap<decltype(Owner::Member)>::value, \
                offsetof(Owner, Member) \
            }); \
        } \
    }; \
    inline static _AutoRegister_##Member _auto_##Member; \
public: