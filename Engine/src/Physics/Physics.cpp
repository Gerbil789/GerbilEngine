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
  // Storing uint64_t directly to avoid Uuid operator< ambiguity
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

  void DispatchCollision(Entity entity, Entity other, CollisionType type, bool isTrigger)
  {
    if (!entity || !entity.Has<ScriptComponent>()) return;

    auto* script = entity.Get<ScriptComponent>().instance;
    if (!script) return;

    // Clean, elegant dispatch using ternary operators
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
    auto view = scene.GetEntities<TransformComponent, ColliderComponent>();

    std::map<std::pair<uint64_t, uint64_t>, bool> currentCollisions;

    // 1. Update all world AABBs
    for (auto& entity : view)
    {
      const auto& transform = entity.Get<TransformComponent>();
      auto& collider = entity.Get<ColliderComponent>();

      const auto& mesh = AssetManager::GetAsset<Engine::Mesh>(collider.meshId);
      collider.worldAABB = CalculateWorldAABB(mesh.aabb, transform.GetWorld());
    }

    // 2. Check for intersections
    for (auto itA = view.begin(); itA != view.end(); ++itA)
    {
      for (auto itB = std::next(itA); itB != view.end(); ++itB)
      {
        Entity entityA = *itA;
        Entity entityB = *itB;

        auto& colA = entityA.Get<ColliderComponent>();
        auto& colB = entityB.Get<ColliderComponent>();

        // Static vs Static will never collide in a meaningful way
        if (colA.type == BodyType::Static && colB.type == BodyType::Static) continue;

        // --- COLLISION CHECK ---
        if (IntersectTest(colA.worldAABB, colB.worldAABB))
        {
          uint64_t idA = (uint64_t)entityA.Get<IdentityComponent>().id;
          uint64_t idB = (uint64_t)entityB.Get<IdentityComponent>().id;
          auto pair = std::minmax(idA, idB);

          bool isTrigger = colA.isTrigger || colB.isTrigger;
          currentCollisions[pair] = isTrigger;

          // If not found in previous frame, it's an Enter event
          if (s_PreviousCollisions.find(pair) == s_PreviousCollisions.end())
          {
            DispatchCollision(entityA, entityB, CollisionType::Enter, isTrigger);
            DispatchCollision(entityB, entityA, CollisionType::Enter, isTrigger);
          }
        }
      }
    }

    // 3. Process Exits
    for (const auto& [pair, wasTrigger] : s_PreviousCollisions)
    {
      if (currentCollisions.find(pair) == currentCollisions.end())
      {
        Entity entityA = scene.GetEntity(pair.first); // Implicitly casts uint64_t back to Uuid if constructor exists
        Entity entityB = scene.GetEntity(pair.second);

        if (entityA) DispatchCollision(entityA, entityB, CollisionType::Exit, wasTrigger);
        if (entityB) DispatchCollision(entityB, entityA, CollisionType::Exit, wasTrigger);
      }
    }

    // 4. Save state for next frame
    s_PreviousCollisions = std::move(currentCollisions);
  }
}