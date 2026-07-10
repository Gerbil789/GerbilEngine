#pragma once

#include "Engine/Core/API.h"
#include <entt/entity/entity.hpp>

namespace Engine
{
  class Scene;

  class ENGINE_API Entity
  {
  public:
		Entity() = default;
    Entity(entt::entity handle, Scene* scene);

		void SetActive(bool active);
    bool IsActive() const;

    void Destroy();

    template<typename T>
    T& AddComponent();

    template<typename T>
    T& GetComponent();

    template<typename T>
    bool HasComponent();

    template<typename T>
    void RemoveComponent();

    entt::entity GetHandle() const { return m_Handle; }
		Scene* GetScene() const { return m_Scene; }

    void SetDirty();

		explicit operator bool() const { return m_Handle != entt::null && m_Scene != nullptr; }
		bool operator==(const Entity& other) const { return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }

  private:
    entt::entity m_Handle{ entt::null };
    Scene* m_Scene = nullptr;
  };
}