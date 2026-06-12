#include "enginepch.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	DrawList DrawList::CreateFromScene(Scene& scene)
	{
		DrawList list;

		entt::registry& registry = scene.GetRegistry();
		auto view = registry.view<IdentityComponent, TransformComponent, MeshComponent>();

		for (auto [entity, identity, transform, mc] : view.each())
		{
			if (!identity.enabled || !mc.meshId) continue;

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

				list.items.emplace_back(DrawItem{ mc.meshId, materialId, i, subMesh.indexCount, subMesh.firstIndex, entity });
			}
		}

		std::sort(list.begin(), list.end(), [](const DrawItem& a, const DrawItem& b)
			{
				if (a.materialId != b.materialId)
				{
					return a.materialId < b.materialId;
				}
				return a.meshId < b.meshId;
			});

		return list;
	}
}