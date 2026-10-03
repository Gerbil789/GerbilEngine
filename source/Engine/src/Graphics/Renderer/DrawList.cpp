#include "enginepch.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Asset/Resources.h"

namespace engine
{
	struct SortableDrawData
	{
		DrawItem item;
		glm::mat4 transform;
	};

	DrawList DrawList::Create(const entt::registry& registry)
	{
		DrawList list;

		auto view = registry.view<MeshComponent, WorldTransformComponent>(entt::exclude<DisabledTag>);

		std::vector<SortableDrawData> tempDrawData;

		tempDrawData.reserve(view.size_hint());

		for (const auto& [entity, mc, wtc] : view.each())
		{
			if (!mc.mesh) continue;

			const auto& subMeshes = mc.mesh.GetSubMeshes();

			for (uint32_t i = 0; i < subMeshes.size(); ++i)
			{
				const auto& subMesh = subMeshes[i];
				engine::MaterialInstance materialInstance{ RESOURCES::MATERIAL_INSTANCE::INVALID };

				if (subMesh.materialIndex < mc.materials.size() && mc.materials[subMesh.materialIndex])
				{
					materialInstance = mc.materials[subMesh.materialIndex];
				}

				tempDrawData.push_back({DrawItem{ mc.mesh, materialInstance, i, subMesh.indexCount, subMesh.firstIndex }, wtc.worldMatrix });
			}
		}

		std::sort(tempDrawData.begin(), tempDrawData.end(), [](const SortableDrawData& a, const SortableDrawData& b)
			{
				if (a.item.material != b.item.material)
				{
					return a.item.material.id < b.item.material.id;
				}
				return a.item.mesh.id < b.item.mesh.id;
			});


		size_t size = tempDrawData.size();
		list.items.reserve(size);
		list.transforms.reserve(size);

		for (const auto& data : tempDrawData)
		{
			list.items.push_back(data.item);
			list.transforms.push_back(data.transform);
		}

		return list;
	}
}