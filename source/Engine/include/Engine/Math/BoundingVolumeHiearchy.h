#pragma once

#include "Engine/Math/AABB.h"
#include <vector>
#include <cstdint>
#include <entt/entity/entity.hpp>

namespace engine
{
  struct Vertex;

  struct alignas(32) BVHNode
  {
    AABB aabb;
    uint32_t leftChildOrEntity; // Left child index (internal) or Entity ID / Triangle Index (leaf)
    uint32_t isLeaf;            // 0 = internal node, 1 = leaf node
  };

  class BoundingVolumeHiearchy
  {
  public:
    std::vector<BVHNode> nodes;

    void Build(const entt::registry& registry); //TLAS
    void Build(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices); //BLAS

  private:
    struct BVHPrimitive
    {
      uint32_t payloadID; // Stores either static_cast<uint32_t>(entity) or triangleIndex
      AABB aabb;
      glm::vec3 centroid;
    };

    std::vector<BVHPrimitive> primitives;
    uint32_t nodesUsed = 0;

    void UpdateNodeBounds(uint32_t nodeIdx, uint32_t firstPrim, uint32_t primCount);
    void Subdivide(uint32_t nodeIdx, uint32_t firstPrim, uint32_t primCount);
  };
}