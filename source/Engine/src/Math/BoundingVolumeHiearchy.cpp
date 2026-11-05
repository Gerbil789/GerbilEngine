#include "enginepch.h"
#include "Engine/Math/BoundingVolumeHiearchy.h"
#include "Engine/Core/Components.h"
#include <algorithm>
#include <limits>

namespace engine
{
  void BoundingVolumeHiearchy::Build(const entt::registry& registry)
  {
    auto view = registry.view<const MeshComponent, const WorldTransformComponent>();

    instances.clear();
    instances.reserve(view.size_hint());

    for (auto [entity, mc, wtc] : view.each())
    {
      const AABB& localAABB = mc.mesh.Get().aabb;

      glm::vec3 corners[8] 
      {
          {localAABB.min.x, localAABB.min.y, localAABB.min.z},
          {localAABB.max.x, localAABB.min.y, localAABB.min.z},
          {localAABB.min.x, localAABB.max.y, localAABB.min.z},
          {localAABB.max.x, localAABB.max.y, localAABB.min.z},
          {localAABB.min.x, localAABB.min.y, localAABB.max.z},
          {localAABB.max.x, localAABB.min.y, localAABB.max.z},
          {localAABB.min.x, localAABB.max.y, localAABB.max.z},
          {localAABB.max.x, localAABB.max.y, localAABB.max.z}
      };

      AABB worldAABB;
      for (int i = 0; i < 8; ++i)
      {
        glm::vec3 worldPos = glm::vec3(wtc.worldMatrix * glm::vec4(corners[i], 1.0f));
        worldAABB.min = glm::min(worldAABB.min, worldPos);
        worldAABB.max = glm::max(worldAABB.max, worldPos);
      }

      glm::vec3 centroid = (worldAABB.min + worldAABB.max) * 0.5f;
      instances.push_back({ entity, worldAABB, centroid });
    }

    nodes.clear();
    if (instances.empty()) return;

    // A binary tree with N leaves has exactly 2N - 1 total nodes.
    nodes.resize(instances.size() * 2 - 1);
    nodesUsed = 1; // Root node starts at index 0

    UpdateNodeBounds(0, 0, static_cast<uint32_t>(instances.size()));
    Subdivide(0, 0, static_cast<uint32_t>(instances.size()));
  }

  void BoundingVolumeHiearchy::UpdateNodeBounds(uint32_t nodeIdx, uint32_t firstInst, uint32_t instCount)
  {
    AABB& aabb = nodes[nodeIdx].aabb;
    aabb.min = glm::vec3((std::numeric_limits<float>::max)());
    aabb.max = glm::vec3((std::numeric_limits<float>::lowest)());

    for (uint32_t i = 0; i < instCount; ++i)
    {
      const AABB& instAABB = instances[firstInst + i].aabb;
      aabb.min = glm::min(aabb.min, instAABB.min);
      aabb.max = glm::max(aabb.max, instAABB.max);
    }
  }

  void BoundingVolumeHiearchy::Subdivide(uint32_t nodeIdx, uint32_t firstInst, uint32_t instCount)
  {
    // Base case: if there's only 1 instance, make this node a leaf
    if (instCount == 1)
    {
      nodes[nodeIdx].isLeaf = 1;
      nodes[nodeIdx].leftChildOrEntity = static_cast<uint32_t>(instances[firstInst].entity);
      return;
    }

    nodes[nodeIdx].isLeaf = 0;

    // 1. Find the longest axis of the current node's bounding box to split along
    glm::vec3 extent = nodes[nodeIdx].aabb.max - nodes[nodeIdx].aabb.min;
    int splitAxis = 0;
    if (extent.y > extent.x) splitAxis = 1;
    if (extent.z > extent[splitAxis]) splitAxis = 2;

    // 2. Compute spatial midpoint split position
    float splitPos = nodes[nodeIdx].aabb.min[splitAxis] + extent[splitAxis] * 0.5f;

    // 3. Partition instances in-place based on their centroid position
    uint32_t i = firstInst;
    uint32_t j = firstInst + instCount - 1;
    while (i <= j)
    {
      if (instances[i].centroid[splitAxis] < splitPos)
      {
        i++;
      }
      else
      {
        std::swap(instances[i], instances[j]);
        if (j == 0) break; // Prevent underflow if j hits 0
        j--;
      }
    }

    // Fallback if all instances ended up on one side
    uint32_t leftCount = i - firstInst;
    if (leftCount == 0 || leftCount == instCount)
    {
      leftCount = instCount / 2;
    }

    // 4. Allocate child nodes sequentially in the flat array
    // Note: Because left child is allocated first and right child second, 
    // the right child is ALWAYS at `leftChildIdx + 1`.
    uint32_t leftChildIdx = nodesUsed++;
    uint32_t rightChildIdx = nodesUsed++;

    nodes[nodeIdx].leftChildOrEntity = leftChildIdx;

    UpdateNodeBounds(leftChildIdx, firstInst, leftCount);
    Subdivide(leftChildIdx, firstInst, leftCount);

    UpdateNodeBounds(rightChildIdx, firstInst + leftCount, instCount - leftCount);
    Subdivide(rightChildIdx, firstInst + leftCount, instCount - leftCount);
  }
}