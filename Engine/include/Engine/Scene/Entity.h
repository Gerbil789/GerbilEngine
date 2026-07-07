#pragma once

#include "Engine/Core/API.h"
#include <cstdint>

namespace Engine
{
  class Scene;

  class ENGINE_API Entity
  {
  public:
		Entity() = default;
    Entity(uint32_t handle, Scene* scene);

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

		uint32_t GetHandle() const { return m_Handle; }
		Scene* GetScene() const { return m_Scene; }
		bool IsValid() const { return m_Handle != 0xFFFFFFFF && m_Scene != nullptr; }

		bool operator==(const Entity& other) const { return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }

  private:
    uint32_t m_Handle = 0xFFFFFFFF;
    Scene* m_Scene = nullptr;
  };
}