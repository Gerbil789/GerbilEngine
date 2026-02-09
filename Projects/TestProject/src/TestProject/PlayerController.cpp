//#include "enginepch.h"
#include "PlayerController.h"
#include <print>
#include "Engine/Core/Input.h"

using namespace Engine;

void PlayerController::OnCreate()
{
	std::println("PlayerController::OnCreate called");
	Self.GetComponent<TransformComponent>().rotation.x = 0.0f;

}

void PlayerController::OnUpdate(float delta)
{
	//std::println("PlayerController::OnUpdate called with delta: {}", delta);

	if(Input::IsKeyPressed(Key::W))
	{
		Self.GetComponent<TransformComponent>().position.y += Speed * delta;
	}

	if(Input::IsKeyPressed(Key::S))
	{
		Self.GetComponent<TransformComponent>().position.y -= Speed * delta;
	}

	if(Input::IsKeyPressed(Key::A))
	{
		Self.GetComponent<TransformComponent>().position.x -= Speed * delta;
	}

	if(Input::IsKeyPressed(Key::D))
	{
		Self.GetComponent<TransformComponent>().position.x += Speed * delta;
	}

	if(Input::IsKeyPressed(Key::Q))
	{
		Self.GetComponent<TransformComponent>().position.z += Speed * delta;
	}

	if(Input::IsKeyPressed(Key::E))
	{
		Self.GetComponent<TransformComponent>().position.z -= Speed * delta;
	}

	Self.GetComponent<TransformComponent>().rotation.x += Speed * delta;
}
