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
		uint32_t modelIndex = 0;

		entt::registry& registry = scene.GetRegistry();
		auto view = registry.view<IdentityComponent, TransformComponent, MeshComponent>();

		view.each([&](auto entity, IdentityComponent& identity, TransformComponent&, MeshComponent& mc)
		{
				if (!identity.enabled || !mc.meshId)
				{
					return;
				}

				Engine::Mesh& mesh = Engine::AssetManager::GetAsset<Mesh>(mc.meshId);
				for (const auto& subMesh : mesh.GetSubMeshes())
				{
					list.items.emplace_back(DrawItem{ entity, &mesh, &subMesh, modelIndex++ });
				}
		});

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