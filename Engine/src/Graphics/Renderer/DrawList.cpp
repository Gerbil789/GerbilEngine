#include "enginepch.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Mesh.h"

namespace Engine
{
	DrawList DrawList::CreateFromScene(Scene* scene)
	{
		DrawList list;

		uint32_t modelIndex = 0;

		const auto& entities = scene->GetEntities<TransformComponent, MeshComponent>();
		list.items.reserve(entities.size());

		for (auto entity : entities)
		{
			auto& mc = entity.Get<MeshComponent>();
			if (!mc.mesh)
			{
				continue;
			}

			for (const auto& subMesh : mc.mesh->GetSubMeshes())
			{
				list.items.push_back({ entity, mc.mesh, &subMesh, modelIndex++ });
			}
		}

		std::sort(list.items.begin(), list.items.end(), [](const DrawItem& a, const DrawItem& b)
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