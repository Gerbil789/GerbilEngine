#pragma once

#include "Engine/Math/AABB.h"
#include <entt/entity/registry.hpp>

namespace engine
{
  struct alignas(32) BVHNode
  {
    AABB aabb;
    uint32_t leftChildOrEntity; // 4 bytes: If internal = index of left child. If leaf = entity ID.
    uint32_t isLeaf;            // 4 bytes: 0 = internal node, 1 = leaf node
  };

  class BoundingVolumeHiearchy
  {
  public:
    std::vector<BVHNode> nodes;

    void Build(const entt::registry& registry);

  private:
    struct BVHInstance
    {
      entt::entity entity;
      AABB aabb;
      glm::vec3 centroid;
    };

    std::vector<BVHInstance> instances;
    uint32_t nodesUsed = 0;

    void UpdateNodeBounds(uint32_t nodeIdx, uint32_t firstInst, uint32_t instCount);
    void Subdivide(uint32_t nodeIdx, uint32_t firstInst, uint32_t instCount);
  };
}