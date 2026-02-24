#pragma once

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"

namespace Engine
{
	struct DrawItem
	{
		Entity entity;
		//Mesh* mesh;
		Mesh* mesh;
		const SubMesh* subMesh;
		//Material* material;
		uint32_t modelIndex; // index into model buffer
	};

	class DrawList
	{
	public:
		std::vector<DrawItem> items;

		inline static DrawList CreateFromScene(Scene* scene)
		{
			DrawList list;

			uint32_t modelIndex = 0;

			const auto& entities = scene->GetEntities<TransformComponent, MeshComponent>();
			list.items.reserve(entities.size());

			for (auto entity : entities)
			{
				auto& mc = entity.GetComponent<MeshComponent>();
				if (!mc.mesh)
				{
					continue;
				}

				for(const auto& subMesh : mc.mesh->GetSubMeshes())
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
	};
}