#include "enginepch.h"
#include "Engine/Math/Raycast.h"
#include "Engine/Core/Components.h"
#include <entt/entity/registry.hpp>

namespace engine
{
  RayHit Raycast(const entt::registry& registry, const Ray& ray)
  {
    RayHit hit;
    hit.distance = (std::numeric_limits<float>::max)();

    auto view = registry.view<const MeshComponent, const WorldTransformComponent>();

    for(auto [entity, mc, wtc] : view.each())
    {
      const AABB& aabb = mc.mesh.GetAABB();

      glm::mat4 inverseModel = glm::inverse(wtc.worldMatrix);

      glm::vec3 localOrigin = glm::vec3(inverseModel * glm::vec4(ray.origin, 1.0f));
      glm::vec3 localDir    = glm::vec3(inverseModel * glm::vec4(ray.direction, 0.0f));

      glm::vec3 invDir = 1.0f / localDir; 
      
      glm::vec3 t0 = (aabb.min - localOrigin) * invDir;
      glm::vec3 t1 = (aabb.max - localOrigin) * invDir;

      glm::vec3 tMin = glm::min(t0, t1);
      glm::vec3 tMax = glm::max(t0, t1);

      float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
      float tFar  = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

      if (tNear > tFar || tFar < 0.0f)
      {
        continue;
      }

      float t = (tNear < 0.0f) ? tFar : tNear;

      if (t < hit.distance)
      {
        hit.entity = entity;
        hit.distance = t;
        
        hit.point = ray.origin + ray.direction * t;
      }
    }

    return hit;
  }
}