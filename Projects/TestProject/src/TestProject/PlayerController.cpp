#include "PlayerController.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Time.h"
#include "Engine/Utility/Color.h"
#include "Engine/Graphics/Mesh.h"

using namespace Engine;

void PlayerController::OnCreate()
{
}

void PlayerController::OnUpdate()
{
	float delta = Time::DeltaTime();

	auto& transform = Self.GetComponent<TransformComponent>();

	// movement
	if (Input::IsKeyPressed(Key::A)) transform.position.x += m_MoveSpeed * delta;
	if (Input::IsKeyPressed(Key::D)) transform.position.x -= m_MoveSpeed * delta;
	if (Input::IsKeyPressed(Key::W)) transform.position.z += m_MoveSpeed * delta;
	if (Input::IsKeyPressed(Key::S)) transform.position.z -= m_MoveSpeed * delta;

	if (Input::IsKeyPressed(Key::Space))
	{
		transform.position.y += m_JumpStrength * delta;
	}
	// gravity
	if(transform.position.y > m_Ground)
	{
		transform.position.y -= Time::DeltaTime() * m_GravityStrength;

	}
	else
	{
		transform.position.y = m_Ground;
	}
}

