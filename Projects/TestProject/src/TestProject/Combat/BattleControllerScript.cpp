#include "BattleControllerScript.h"
#include "GridInteractionSystem.h"
#include "GameContext.h"

namespace
{
  GridInteractionSystem gridInteractionSystem;
}

void BattleControllerScript::OnStart()
{
	GameContext::grid = Grid(m_GridWidth, m_GridHeight);

  //entt::registry& registry = m_Scene->GetRegistry();
  //registry.emplace_or_replace<Engine::MeshComponent>(m_Entity, Engine::MeshComponent{ m_TileMesh.id, { m_TileMaterial.id } });

  if(m_Entity.HasComponent<Engine::MeshComponent>())
  {
    Engine::MeshComponent& meshComp = m_Entity.GetComponent<Engine::MeshComponent>();
    meshComp.meshId = m_TileMesh.id;
    meshComp.materials = { m_TileMaterial.id };
  }
  else
  {
    Engine::MeshComponent& meshComp = m_Entity.AddComponent<Engine::MeshComponent>();
		meshComp.meshId = m_TileMesh.id;
		meshComp.materials = { m_TileMaterial.id };
	}

	Engine::AssetManager::GetAsset<Engine::Material>(m_TileMaterial.id).SetParameter("tiling", glm::vec2(m_GridWidth, m_GridHeight));


	Engine::TransformComponent& transform = m_Entity.GetComponent<Engine::TransformComponent>();
	transform.scale = { static_cast<float>(m_GridWidth) * 2, 1.0f, static_cast<float>(m_GridHeight) * 2 };
  m_Entity.AddTag<Engine::TransformDirty>();

  gridInteractionSystem.Initialize();

  LOG_INFO("BattleControllerScript created and initialized.");
}


void BattleControllerScript::OnUpdate()
{
	gridInteractionSystem.Update();
  m_TileMaterial.Get().SetParameter("hoveredTile", GameContext::mousePosition);


  //if (m_combatManager)
  //{
  //  m_combatManager->Update();
  //}
}


void BattleControllerScript::OnDestroy()
{
  LOG_INFO("BattleControllerScript destroyed and cleaned up.");
}
