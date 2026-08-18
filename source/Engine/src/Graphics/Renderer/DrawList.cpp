#include "enginepch.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Resources.h"

namespace Engine
{
	struct SortableDrawData
	{
		DrawItem item;
		glm::mat4 transform;
	};


	DrawList DrawList::CreateFromScene(Scene& scene)
	{
		DrawList list;
		entt::registry& registry = scene.GetRegistry();

		auto view = registry.view<MeshComponent, WorldTransformComponent, IdentityComponent>(entt::exclude<DisabledTag>);

		std::vector<SortableDrawData> tempDrawData;

		tempDrawData.reserve(view.size_hint());

		for (auto&& [entity, mc, wtc, ic] : view.each())
		{
			if (!mc.meshId) continue;

			const Engine::Mesh& mesh = Engine::AssetManager::GetAsset<Mesh>(mc.meshId);
			const auto& subMeshes = mesh.GetSubMeshes();

			for (uint32_t i = 0; i < subMeshes.size(); ++i)
			{
				const auto& subMesh = subMeshes[i];
				Engine::Uuid materialId{ RESOURCES::MATERIAL::PINK };

				if (subMesh.materialIndex < mc.materials.size() && mc.materials[subMesh.materialIndex])
				{
					materialId = mc.materials[subMesh.materialIndex];
				}

				tempDrawData.push_back({DrawItem{ mc.meshId, materialId, i, subMesh.indexCount, subMesh.firstIndex, ic.id }, wtc.worldMatrix });
			}
		}

		std::sort(tempDrawData.begin(), tempDrawData.end(), [](const SortableDrawData& a, const SortableDrawData& b)
			{
				if (a.item.materialId != b.item.materialId)
				{
					return a.item.materialId < b.item.materialId;
				}
				return a.item.meshId < b.item.meshId;
			});

		auto& items = const_cast<std::vector<DrawItem>&>(list.GetItems());
		auto& transforms = const_cast<std::vector<glm::mat4>&>(list.GetTransforms());

		items.reserve(tempDrawData.size());
		transforms.reserve(tempDrawData.size());

		for (const auto& data : tempDrawData)
		{
			items.push_back(data.item);
			transforms.push_back(data.transform);
		}

		return list;
	}
}