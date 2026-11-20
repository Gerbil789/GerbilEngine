#include "enginepch.h"
#include "engine/graphics/renderer/draw_list.h"
#include "engine/core/components.h"
#include "engine/math/frustum.h"

namespace engine
{
  DrawList DrawList::Create(const Scene& scene, const component::Camera* camera)
  {
    DrawList list;

    auto frustum = ExtractFrustumPlanesZO(camera->viewProjectionMatrix);

    auto view = scene.registry.view<component::Mesh, component::WorldTransform>(entt::exclude<tag::Disabled>);

    list.items.reserve(view.size_hint());
    list.transforms.reserve(view.size_hint());

    for (const auto& [entity, mc, wtc] : view.each())
    {
      if (mc.mesh == mesh::Empty) continue;

      list.totalDrawItems++;
      const Mesh& mesh = mc.mesh.Get();

      AABB worldAABB = TransformAABB(mesh.aabb, wtc.worldMatrix);

      if (!IsAABBInFrustum(worldAABB, frustum))
      {
        list.culledCount++;
        continue;
      }

      const auto& submeshes = mesh.submeshes;

      for (uint32_t i = 0; i < submeshes.size(); ++i)
      {
        const auto& subMesh = submeshes[i];
        assert(subMesh.materialIndex < mc.materialInstances.size() && "Material index out of bounds!");

        engine::MaterialInstanceHandle materialInstance = mc.materialInstances[subMesh.materialIndex];
        list.items.emplace_back(mc.mesh, materialInstance, i, subMesh.indexCount, subMesh.firstIndex);
        list.transforms.push_back(wtc.worldMatrix);
      }
    }

    const size_t count = list.items.size();
    if (count <= 1) return list;

    struct SortEntry
    {
      uint64_t key;
      uint32_t index;
    };

    static thread_local std::vector<SortEntry> sortEntries;
    static thread_local std::vector<DrawItem> sortedItems;
    static thread_local std::vector<glm::mat4> sortedTransforms;

    sortEntries.clear();
    sortEntries.reserve(count);

    for (uint32_t i = 0; i < static_cast<uint32_t>(count); ++i)
    {
      const auto& item = list.items[i];
      const uint64_t key = (static_cast<uint64_t>(item.materialInstance.id) << 32) | static_cast<uint64_t>(item.mesh.id);
      sortEntries.push_back({ key, i });
    }

    std::sort(sortEntries.begin(), sortEntries.end(), [](const SortEntry& a, const SortEntry& b)
      {
        return a.key < b.key;
      });

    sortedItems.resize(count);
    sortedTransforms.resize(count);

    for (size_t i = 0; i < count; ++i)
    {
      const uint32_t src = sortEntries[i].index;
      sortedItems[i] = list.items[src];
      sortedTransforms[i] = list.transforms[src];
    }

    list.items.swap(sortedItems);
    list.transforms.swap(sortedTransforms);

    return list;
  }
}