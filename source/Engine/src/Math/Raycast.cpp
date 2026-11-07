#include "enginepch.h"
#include "Engine/Math/Raycast.h"
#include "Engine/Core/Components.h"
#include <entt/entity/registry.hpp>

namespace engine
{
  inline bool IntersectRayAABB(const Ray& ray, const AABB& aabb, float& outT)
  {
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; ++i)
    {
      float invDir = 1.0f / ray.direction[i];
      float t1 = (aabb.min[i] - ray.origin[i]) * invDir;
      float t2 = (aabb.max[i] - ray.origin[i]) * invDir;

      if (t1 > t2) std::swap(t1, t2);

      tMin = glm::max(t1, tMin);
      tMax = glm::min(t2, tMax);

      if (tMin > tMax) return false;
    }

    if (tMax < 0.0f) return false;

    outT = (tMin >= 0.0f) ? tMin : tMax;
    return true;
  }

  RayHit Raycast(const Scene& scene, const Ray& worldRay)
  {
    RayHit hit;
    hit.distance = (std::numeric_limits<float>::max)();
    hit.entity = entt::null;

    const BoundingVolumeHiearchy& tlas = scene.bvh;
    if (tlas.nodes.empty()) return hit;

    uint32_t tlasStack[64];
    int tlasStackPtr = 0;
    tlasStack[tlasStackPtr++] = 0;

    while (tlasStackPtr > 0)
    {
      uint32_t nodeIdx = tlasStack[--tlasStackPtr];
      const BVHNode& node = tlas.nodes[nodeIdx];

      float tAABB = 0.0f;
      if (!IntersectRayAABB(worldRay, node.aabb, tAABB)) continue;
      if (tAABB >= hit.distance) continue;

      if (node.isLeaf)
      {
        entt::entity entity = static_cast<entt::entity>(node.leftChildOrEntity);

        const auto& meshComp = scene.registry.get<component::Mesh>(entity);
        const auto& transformComp = scene.registry.get<component::WorldTransform>(entity);

        const Mesh& mesh = meshComp.mesh.Get();
        const auto& blas = mesh.bvh;
        if (blas.nodes.empty()) continue;

        glm::mat4 inverseTransform = glm::inverse(transformComp.worldMatrix);
        Ray localRay;
        localRay.origin = glm::vec3(inverseTransform * glm::vec4(worldRay.origin, 1.0f));
        
        localRay.direction = glm::vec3(inverseTransform * glm::vec4(worldRay.direction, 0.0f));
        float rayLength = glm::length(localRay.direction);
        localRay.direction = glm::normalize(localRay.direction);

        uint32_t blasStack[64];
        int blasStackPtr = 0;
        blasStack[blasStackPtr++] = 0;

        while (blasStackPtr > 0)
        {
          uint32_t blasNodeIdx = blasStack[--blasStackPtr];
          const BVHNode& blasNode = blas.nodes[blasNodeIdx];

          float tBlasAABB = 0.0f;
          if (!IntersectRayAABB(localRay, blasNode.aabb, tBlasAABB)) continue;

          if (blasNode.isLeaf)
          {
            float worldT = tBlasAABB / rayLength;

            if (worldT < hit.distance)
            {
              hit.distance = worldT;
              hit.entity = entity;
              hit.point = worldRay.origin + worldRay.direction * worldT;
            }
          }
          else
          {
            uint32_t leftIdx = blasNode.leftChildOrEntity;
            uint32_t rightIdx = leftIdx + 1;

            blasStack[blasStackPtr++] = rightIdx;
            blasStack[blasStackPtr++] = leftIdx;
          }
        }
      }
      else
      {
        uint32_t leftIdx = node.leftChildOrEntity;
        uint32_t rightIdx = leftIdx + 1;

        tlasStack[tlasStackPtr++] = rightIdx;
        tlasStack[tlasStackPtr++] = leftIdx;
      }
    }

    return hit;
  }
}