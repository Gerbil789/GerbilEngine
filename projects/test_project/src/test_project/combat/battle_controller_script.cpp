#include "battle_controller_script.h"
//#include "grid_interaction_system.h"
//#include "game_context.h"
//
//namespace
//{
//  GridInteractionsystem gridInteractionsystem;
//}
//
//void BattleControllerScript::OnStart()
//{
//	GameContext::grid = Grid(m_gridWidth, m_gridHeight);
//
//  //entt::registry& registry = m_scene->GetRegistry();
//  //registry.emplace_or_replace<engine::MeshComponent>(m_entity, engine::MeshComponent{ m_tileMesh.id, { m_tileMaterial.id } });
//
//  if(m_entity.HasComponent<engine::MeshComponent>())
//  {
//    engine::MeshComponent& meshComp = m_entity.GetComponent<engine::MeshComponent>();
//    meshComp.mesh = m_tileMesh;
//    meshComp.materials = { m_tileMaterial };
//  }
//  else
//  {
//    engine::MeshComponent& meshComp = m_entity.AddComponent<engine::MeshComponent>();
//		meshComp.mesh = m_tileMesh;
//		meshComp.materials = { m_tileMaterial };
//	}
//
//	engine::assetManager::Getasset(m_tileMaterial).SetParameter("tiling", glm::vec2(m_gridWidth, m_gridHeight));
//
//
//	engine::TransformComponent& transform = m_entity.GetComponent<engine::TransformComponent>();
//	transform.scale = { static_cast<float>(m_gridWidth) * 2, 1.0f, static_cast<float>(m_gridHeight) * 2 };
//  m_entity.AddTag<engine::TransformDirty>();
//
//  gridInteractionsystem.Initialize();
//
//  LOG_INFO("BattleControllerScript created and initialized.");
//}
//
//
//void BattleControllerScript::OnUpdate()
//{
//	gridInteractionsystem.Update();
//  auto& mat = engine::assetManager::Getasset(m_tileMaterial);
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
