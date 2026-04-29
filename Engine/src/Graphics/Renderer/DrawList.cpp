#include "enginepch.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	DrawList DrawList::CreateFromScene(Scene* scene)
	{
		DrawList list;
		uint32_t modelIndex = 0;

		const auto& entities = scene->GetEntities<TransformComponent, MeshComponent>();
		list.items.reserve(entities.size());

		for (const auto& entity : entities)
		{
			const auto& mc = entity.Get<MeshComponent>();
			if (!mc.mesh)
			{
				continue;
			}

			Engine::Mesh& mesh = Engine::AssetManager::GetAsset<Mesh>(mc.mesh);

			for (const auto& subMesh : mesh.GetSubMeshes())
			{
				list.items.emplace_back(DrawItem{ entity, &mesh, &subMesh, modelIndex++ });
			}
		}

		std::sort(list.begin(), list.end(), [](const DrawItem& a, const DrawItem& b)
			{
				if (a.subMesh->materialIndex != b.subMesh->materialIndex)
				{
					return a.subMesh->materialIndex < b.subMesh->materialIndex;
				}
				return a.subMesh < b.subMesh;
			});

		return list;
	}
}