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

  entt::registry& registry = m_Scene->GetRegistry();
  registry.emplace_or_replace<Engine::MeshComponent>(m_Entity, Engine::MeshComponent{ m_TileMesh.id, { m_TileMaterial.id } });

	Engine::AssetManager::GetAsset<Engine::Material>(m_TileMaterial.id).SetParameter("tiling", glm::vec2(m_GridWidth, m_GridHeight));


  Engine::TransformComponent& transform = registry.get<Engine::TransformComponent>(m_Entity);
	transform.scale = { static_cast<float>(m_GridWidth) * 2, 1.0f, static_cast<float>(m_GridHeight) * 2 };

  gridInteractionSystem.Initialize();

  LOG_INFO("BattleControllerScript created and initialized.");
}


void BattleControllerScript::OnUpdate()
{
	gridInteractionSystem.Update(*m_Scene->GetActiveCamera());
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
