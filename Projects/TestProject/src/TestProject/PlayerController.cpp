#include "enginepch.h"
#include "PlayerController.h"
#include "Engine/Scene/Entity.h"
#include <print>

void Player_OnCreate(PlayerController& controller, Engine::Entity entity)
{
	std::println("Player_OnCreate called for entity: {}", entity.GetName());
    // Initialize player controller
}

void Player_OnUpdate(PlayerController& controller, Engine::Entity entity, float deltaTime)
{
	//std::println("Player_OnUpdate called for entity: {} with deltaTime: {}", entity.GetName(), deltaTime);

	entity.GetComponent<Engine::TransformComponent>().rotation.y += (controller.Speed + 1.0f) * deltaTime;
}
