#include "SpawnEntitiesScript.h"
#include "Engine/Core/Components.h"

void SpawnEntitiesScript::OnStart()
{
	Engine::SceneAsset* scene = m_Entity.GetScene();

	Engine::HierarchyComponent& parentHiearchy = m_Entity.GetComponent<Engine::HierarchyComponent>();

  for(int x = 0; x < m_GridWidth; ++x)
  {
    for(int y = 0; y < m_GridHeight; ++y)
    {
      Engine::Entity entity = scene->CreateEntity<Engine::TransformComponent, Engine::WorldTransformComponent>("SpawnedEntity");
      Engine::TransformComponent& transform = entity.GetComponent<Engine::TransformComponent>();
      transform.position = { x * m_Offset, 0.0f, y * m_Offset };
      Engine::MeshComponent& meshComp = entity.AddComponent<Engine::MeshComponent>();
      meshComp.mesh = m_Mesh;
      meshComp.materials = { m_Material };
			Engine::HierarchyComponent& hierarchy = entity.GetComponent<Engine::HierarchyComponent>();
			hierarchy.parent = m_Entity.GetHandle();
			parentHiearchy.children.push_back(entity.GetHandle());

      entity.AddTag<Engine::TransformDirty>();
    }
	}
}


void SpawnEntitiesScript::OnUpdate()
{

}

void SpawnEntitiesScript::OnDestroy()
{
	auto& parentHierarchy = m_Entity.GetComponent<Engine::HierarchyComponent>();

  for(auto child : parentHierarchy.children)
  {
    Engine::Entity childEntity(child, m_Entity.GetScene());

		//auto& hierarchy = childEntity.GetComponent<Engine::HierarchyComponent>();

    childEntity.Destroy();
	}

	parentHierarchy.children.clear();
}