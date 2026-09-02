#pragma once

#include <entt/entity/entity.hpp>

namespace engine
{
  class SceneAsset;

  class Entity
  {
  public:
		Entity() = default;
    Entity(entt::entity handle, SceneAsset* scene);

		void SetActive(bool active);
    bool IsActive() const;

    void Destroy();

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args);

    template<typename T>
    void AddTag();

    template<typename T>
    bool HasTag();

    template<typename T>
    void RemoveTag();

    template<typename T>
    T& GetOrAddComponent();

    template<typename T>
    T& GetComponent();

    template<typename T>
    T* TryGetComponent();

    template<typename T>
    bool HasComponent();

    template<typename T>
    void RemoveComponent();

    entt::entity GetHandle() const { return m_Handle; }
    SceneAsset* GetScene() const { return m_Scene; }

		explicit operator bool() const { return m_Handle != entt::null && m_Scene != nullptr; }
		bool operator==(const Entity& other) const { return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }

  private:
    entt::entity m_Handle{ entt::null };
    SceneAsset* m_Scene = nullptr;
  };
}