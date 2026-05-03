#include "enginepch.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Script/Script.h"
#include <map>

namespace Engine
{
  static std::map<std::pair<uint64_t, uint64_t>, bool> s_PreviousCollisions;

  AABB CalculateWorldAABB(const AABB& local, const glm::mat4& transformMatrix)
  {
    glm::vec3 corners[8] = {
        {local.min.x, local.min.y, local.min.z}, {local.max.x, local.min.y, local.min.z},
        {local.min.x, local.max.y, local.min.z}, {local.max.x, local.max.y, local.min.z},
        {local.min.x, local.min.y, local.max.z}, {local.max.x, local.min.y, local.max.z},
        {local.min.x, local.max.y, local.max.z}, {local.max.x, local.max.y, local.max.z}
    };

    glm::vec3 worldMin(FLT_MAX);
    glm::vec3 worldMax(-FLT_MAX);

    for (const auto& corner : corners)
    {
      glm::vec3 worldPos = glm::vec3(transformMatrix * glm::vec4(corner, 1.0f));
      worldMin = glm::min(worldMin, worldPos);
      worldMax = glm::max(worldMax, worldPos);
    }

    return { worldMin, worldMax };
  }

  bool IntersectTest(const AABB& a, const AABB& b)
  {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
      (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
      (a.min.z <= b.max.z && a.max.z >= b.min.z);
  }

  enum class CollisionType { Enter, Exit };

  void DispatchCollision(entt::registry& registry, entt::entity entity, entt::entity other, CollisionType type, bool isTrigger)
  {
		auto scriptComponent = registry.try_get<ScriptComponent>(entity);
		if (!scriptComponent) return;

    auto* script = scriptComponent->instance;
    if (!script) return;

    if (isTrigger)
    {
      type == CollisionType::Enter ? script->OnTriggerEnter(other) : script->OnTriggerExit(other);
    }
    else
    {
      type == CollisionType::Enter ? script->OnCollisionEnter(other) : script->OnCollisionExit(other);
    }
  }

  void PhysicsSystem::Update()
  {
    Scene& scene = SceneManager::GetActiveScene();
		entt::registry& registry = scene.GetRegistry();

		auto view = registry.view<TransformComponent, ColliderComponent>();

    std::map<std::pair<uint64_t, uint64_t>, bool> currentCollisions;

		for (auto [entity, transform, collider] : view.each())
      {
        const auto& mesh = AssetManager::GetAsset<Engine::Mesh>(collider.meshId);
        collider.worldAABB = CalculateWorldAABB(mesh.aabb, transform.worldMatrix);
		};


    for (auto itA = view.begin(); itA != view.end(); ++itA)
    {
      for (auto itB = std::next(itA); itB != view.end(); ++itB)
      {
        entt::entity entityA = *itA;
        entt::entity entityB = *itB;

				auto& colA = registry.get<ColliderComponent>(entityA);
				auto& colB = registry.get<ColliderComponent>(entityB);

        // Static vs Static will never collide in a meaningful way
        if (colA.type == BodyType::Static && colB.type == BodyType::Static) continue;

        // --- COLLISION CHECK ---
        if (IntersectTest(colA.worldAABB, colB.worldAABB))
        {
					uint64_t idA = registry.get<IdentityComponent>(entityA).id;
          uint64_t idB = registry.get<IdentityComponent>(entityB).id;
          auto pair = std::minmax(idA, idB);

          bool isTrigger = colA.isTrigger || colB.isTrigger;
          currentCollisions[pair] = isTrigger;

          // If not found in previous frame, it's an Enter event
          if (s_PreviousCollisions.find(pair) == s_PreviousCollisions.end())
          {
            DispatchCollision(registry, entityA, entityB, CollisionType::Enter, isTrigger);
            DispatchCollision(registry, entityB, entityA, CollisionType::Enter, isTrigger);
          }
        }
      }
    }

    // 3. Process Exits
    for (const auto& [pair, wasTrigger] : s_PreviousCollisions)
    {
      if (currentCollisions.find(pair) == currentCollisions.end())
      {
        entt::entity entityA = scene.GetEntity(pair.first); // Implicitly casts uint64_t back to Uuid if constructor exists
        entt::entity entityB = scene.GetEntity(pair.second);

        if (entityA != entt::null) DispatchCollision(registry, entityA, entityB, CollisionType::Exit, wasTrigger);
        if (entityB != entt::null) DispatchCollision(registry, entityB, entityA, CollisionType::Exit, wasTrigger);
      }
    }

    // 4. Save state for next frame
    s_PreviousCollisions = std::move(currentCollisions);
  }
}