#include "enginepch.h"
#include "Engine/Math/BoundingVolumeHiearchy.h"
#include "Engine/Core/Components.h"
#include <algorithm>
#include <limits>
#include <entt/entity/registry.hpp>

namespace engine
{
  void BoundingVolumeHiearchy::Build(const entt::registry& registry)
  {
    auto view = registry.view<const component::Mesh, const component::WorldTransform>();

    primitives.clear();
    primitives.reserve(view.size_hint());

    for (auto [entity, mc, wtc] : view.each())
    {
      const AABB& localAABB = mc.mesh.Get().aabb;

      glm::vec3 corners[8]
      {
          { localAABB.min.x, localAABB.min.y, localAABB.min.z },
          { localAABB.max.x, localAABB.min.y, localAABB.min.z },
          { localAABB.min.x, localAABB.max.y, localAABB.min.z },
          { localAABB.max.x, localAABB.max.y, localAABB.min.z },
          { localAABB.min.x, localAABB.min.y, localAABB.max.z },
          { localAABB.max.x, localAABB.min.y, localAABB.max.z },
          { localAABB.min.x, localAABB.max.y, localAABB.max.z },
          { localAABB.max.x, localAABB.max.y, localAABB.max.z }
      };

      AABB worldAABB;
      for (int i = 0; i < 8; ++i)
      {
        glm::vec3 worldPos = glm::vec3(wtc.worldMatrix * glm::vec4(corners[i], 1.0f));
        worldAABB.min = glm::min(worldAABB.min, worldPos);
        worldAABB.max = glm::max(worldAABB.max, worldPos);
      }

      glm::vec3 centroid = (worldAABB.min + worldAABB.max) * 0.5f;
      uint32_t entityID = static_cast<uint32_t>(entity);
      primitives.push_back({ entityID, worldAABB, centroid });
    }

    nodes.clear();
    if (primitives.empty()) return;

    nodes.resize(primitives.size() * 2 - 1);
    nodesUsed = 1;

    UpdateNodeBounds(0, 0, static_cast<uint32_t>(primitives.size()));
    Subdivide(0, 0, static_cast<uint32_t>(primitives.size()));
  }

  void BoundingVolumeHiearchy::Build(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
  {
    size_t triangleCount = indices.size() / 3;
    primitives.clear();
    if (triangleCount == 0) return;

    primitives.reserve(triangleCount);

    for (size_t i = 0; i < triangleCount; ++i)
    {
      uint32_t idx0 = indices[i * 3 + 0];
      uint32_t idx1 = indices[i * 3 + 1];
      uint32_t idx2 = indices[i * 3 + 2];

      const glm::vec3& v0 = vertices[idx0].position;
      const glm::vec3& v1 = vertices[idx1].position;
      const glm::vec3& v2 = vertices[idx2].position;

      AABB triAABB;
      triAABB.min = glm::min(v0, glm::min(v1, v2));
      triAABB.max = glm::max(v0, glm::max(v1, v2));

      glm::vec3 centroid = (triAABB.min + triAABB.max) * 0.5f;
      primitives.push_back({ static_cast<uint32_t>(i), triAABB, centroid });
    }

    nodes.clear();
    nodes.resize(primitives.size() * 2 - 1);
    nodesUsed = 1;

    UpdateNodeBounds(0, 0, static_cast<uint32_t>(primitives.size()));
    Subdivide(0, 0, static_cast<uint32_t>(primitives.size()));
  }

  void BoundingVolumeHiearchy::UpdateNodeBounds(uint32_t nodeIdx, uint32_t firstPrim, uint32_t primCount)
  {
    AABB& aabb = nodes[nodeIdx].aabb;
    aabb.min = glm::vec3((std::numeric_limits<float>::max)());
    aabb.max = glm::vec3((std::numeric_limits<float>::lowest)());

    for (uint32_t i = 0; i < primCount; ++i)
    {
      const AABB& primAABB = primitives[firstPrim + i].aabb;
      aabb.min = glm::min(aabb.min, primAABB.min);
      aabb.max = glm::max(aabb.max, primAABB.max);
    }
  }

  void BoundingVolumeHiearchy::Subdivide(uint32_t nodeIdx, uint32_t firstPrim, uint32_t primCount)
  {
    if (primCount == 1)
    {
      nodes[nodeIdx].isLeaf = 1;
      nodes[nodeIdx].leftChildOrEntity = primitives[firstPrim].payloadID;
      return;
    }

    nodes[nodeIdx].isLeaf = 0;

    glm::vec3 extent = nodes[nodeIdx].aabb.max - nodes[nodeIdx].aabb.min;
    int splitAxis = 0;
    if (extent.y > extent.x) splitAxis = 1;
    if (extent.z > extent[splitAxis]) splitAxis = 2;

    float splitPos = nodes[nodeIdx].aabb.min[splitAxis] + extent[splitAxis] * 0.5f;

    uint32_t i = firstPrim;
    uint32_t j = firstPrim + primCount - 1;
    while (i <= j)
    {
      if (primitives[i].centroid[splitAxis] < splitPos)
      {
        i++;
      }
      else
      {
        std::swap(primitives[i], primitives[j]);
        if (j == 0) break;
        j--;
      }
    }

    uint32_t leftCount = i - firstPrim;
    if (leftCount == 0 || leftCount == primCount)
    {
      leftCount = primCount / 2;
    }

    uint32_t leftChildIdx = nodesUsed++;
    uint32_t rightChildIdx = nodesUsed++;

    nodes[nodeIdx].leftChildOrEntity = leftChildIdx;

    UpdateNodeBounds(leftChildIdx, firstPrim, leftCount);
    Subdivide(leftChildIdx, firstPrim, leftCount);

    UpdateNodeBounds(rightChildIdx, firstPrim + leftCount, primCount - leftCount);
    Subdivide(rightChildIdx, firstPrim + leftCount, primCount - leftCount);
  }
}