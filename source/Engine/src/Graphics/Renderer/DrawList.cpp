#include "enginepch.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Core/Components.h"
#include <numeric>
#include <cassert>

namespace engine
{
  DrawList DrawList::Create(const entt::registry& registry)
  {
    DrawList list;

    auto view = registry.view<MeshComponent, WorldTransformComponent>(entt::exclude<DisabledTag>);

    list.items.reserve(view.size_hint());
    list.transforms.reserve(view.size_hint());

    for (const auto& [entity, mc, wtc] : view.each())
    {
      if (mc.mesh == Mesh::Empty) continue;

      const auto& subMeshes = mc.mesh.Get().subMeshes;

      for (uint32_t i = 0; i < subMeshes.size(); ++i)
      {
        const auto& subMesh = subMeshes[i];

        assert(subMesh.materialIndex < mc.materialInstances.size() && "Material index out of bounds!");
        engine::MaterialInstance materialInstance = mc.materialInstances[subMesh.materialIndex];

        list.items.emplace_back(mc.mesh, materialInstance, i, subMesh.indexCount, subMesh.firstIndex);
        list.transforms.push_back(wtc.worldMatrix);
      }
    }

    const size_t count = list.items.size();
    if (count <= 1) return list;

    std::vector<uint32_t> indices(count);
    std::iota(indices.begin(), indices.end(), 0u);

    std::sort(indices.begin(), indices.end(), [&list](uint32_t a, uint32_t b)
    {
      const auto& itemA = list.items[a];
      const auto& itemB = list.items[b];

      if (itemA.materialInstance != itemB.materialInstance)
      {
        return itemA.materialInstance.id < itemB.materialInstance.id;
      }
      return itemA.mesh.id < itemB.mesh.id;
    });

    std::vector<bool> visited(count, false);
    for (size_t i = 0; i < count; ++i)
    {
      if (visited[i] || indices[i] == i) continue;

      size_t current = i;
      while (!visited[current])
      {
        visited[current] = true;
        size_t next = indices[current];

        if (!visited[next])
        {
          std::swap(list.items[i], list.items[next]);
          std::swap(list.transforms[i], list.transforms[next]);
          current = next;
        }
      }
    }

    return list;
  }
}