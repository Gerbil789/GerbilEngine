#include "BattleControllerScript.h"
//#include "GridInteractionSystem.h"
//#include "GameContext.h"
//
//namespace
//{
//  GridInteractionSystem gridInteractionSystem;
//}
//
//void BattleControllerScript::OnStart()
//{
//	GameContext::grid = Grid(m_gridWidth, m_gridHeight);
//
//  //entt::registry& registry = m_scene->GetRegistry();
//  //registry.emplace_or_replace<Engine::MeshComponent>(m_entity, Engine::MeshComponent{ m_tileMesh.id, { m_tileMaterial.id } });
//
//  if(m_entity.HasComponent<Engine::MeshComponent>())
//  {
//    Engine::MeshComponent& meshComp = m_entity.GetComponent<Engine::MeshComponent>();
//    meshComp.mesh = m_tileMesh;
//    meshComp.materials = { m_tileMaterial };
//  }
//  else
//  {
//    Engine::MeshComponent& meshComp = m_entity.AddComponent<Engine::MeshComponent>();
//		meshComp.mesh = m_tileMesh;
//		meshComp.materials = { m_tileMaterial };
//	}
//
//	Engine::AssetManager::GetAsset(m_tileMaterial).SetParameter("tiling", glm::vec2(m_gridWidth, m_gridHeight));
//
//
//	Engine::TransformComponent& transform = m_entity.GetComponent<Engine::TransformComponent>();
//	transform.scale = { static_cast<float>(m_gridWidth) * 2, 1.0f, static_cast<float>(m_gridHeight) * 2 };
//  m_entity.AddTag<Engine::TransformDirty>();
//
//  gridInteractionSystem.Initialize();
//
//  LOG_INFO("BattleControllerScript created and initialized.");
//}
//
//
//void BattleControllerScript::OnUpdate()
//{
//	gridInteractionSystem.Update();
//  auto& mat = Engine::AssetManager::GetAsset(m_tileMaterial);
//  mat.SetParameter("hoveredTile", GameContext::mousePosition);
//
//
//  //if (m_combatManager)
//  //{
//  //  m_combatManager->Update();
//  //}
//}
//
//
//void BattleControllerScript::OnDestroy()
//{
//  LOG_INFO("BattleControllerScript destroyed and cleaned up.");
//}
