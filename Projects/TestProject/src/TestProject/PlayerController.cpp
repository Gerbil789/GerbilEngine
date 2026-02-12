#include "PlayerController.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Graphics/Material.h"


using namespace Engine;

void PlayerController::OnCreate()
{
}


void PlayerController::OnUpdate(float delta)
{
	ProcessInput(delta);

	if (Input::IsKeyPressed(Key::Tab))
	{
		Material* mat = Self.GetComponent<MeshComponent>().material;
		mat->SetTexture("Albedo", TestTexture);

		LOG_INFO("Tab pressed, setting material");

	}


}


void PlayerController::ProcessInput(float delta)
{
	auto& transform = Self.GetComponent<TransformComponent>();

	if (Rotate)
	{
		transform.rotation.x += RotateSpeed * delta;
	}

	if (Input::IsKeyPressed(Key::Space))
	{
		transform.position.y += MoveSpeed * delta;
	}

	if (Input::IsKeyPressed(Key::LeftControl))
	{
		transform.position.y -= MoveSpeed * delta;
	}

	if (Input::IsKeyPressed(Key::A))
	{
		transform.position.x -= MoveSpeed * delta;
	}

	if (Input::IsKeyPressed(Key::D))
	{
		transform.position.x += MoveSpeed * delta;
	}

	if (Input::IsKeyPressed(Key::W))
	{
		transform.position.z += MoveSpeed * delta;
	}

	if (Input::IsKeyPressed(Key::S))
	{
		transform.position.z -= MoveSpeed * delta;
	}
}

