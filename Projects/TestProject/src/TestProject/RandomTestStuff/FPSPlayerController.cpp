#include "FPSPlayerController.h"
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
#include "Engine/Scene/SceneManager.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/Event.h"

using namespace Engine;

void FPSPlayerController::OnStart()
{
	Input::SetCursorMode(Input::CursorMode::Disabled);
	m_LastMouseX = Input::GetMousePosition().x;
	m_LastMouseY = Input::GetMousePosition().y;
}

void FPSPlayerController::OnUpdate()
{
	float delta = Time::DeltaTime();
	auto& transform = GetComponent<TransformComponent>();

	float yawRadians = glm::radians(transform.rotation.y);

	float forwardX = std::sin(yawRadians);
	float forwardZ = std::cos(yawRadians);

	float rightX = std::cos(yawRadians);
	float rightZ = -std::sin(yawRadians);

	// movement
	if (Input::IsKeyDown(Key::W))
	{
		transform.position.x += forwardX * m_MoveSpeed * delta;
		transform.position.z += forwardZ * m_MoveSpeed * delta;
	}
	if (Input::IsKeyDown(Key::S))
	{
		transform.position.x -= forwardX * m_MoveSpeed * delta;
		transform.position.z -= forwardZ * m_MoveSpeed * delta;
	}
	if (Input::IsKeyDown(Key::D))
	{
		transform.position.x += rightX * m_MoveSpeed * delta;
		transform.position.z += rightZ * m_MoveSpeed * delta;
	}
	if (Input::IsKeyDown(Key::A))
	{
		transform.position.x -= rightX * m_MoveSpeed * delta;
		transform.position.z -= rightZ * m_MoveSpeed * delta;
	}

	// gravity
	m_VelocityY -= m_GravityStrength * delta;
	transform.position.y += m_VelocityY * delta;
	if (transform.position.y <= m_Ground)
	{
		transform.position.y = m_Ground; 
		m_VelocityY = 0.0f;
	}

	m_Scene->GetRegistry().patch<Engine::TransformComponent>(m_Entity);
}

void FPSPlayerController::OnDestroy()
{

}

void FPSPlayerController::OnEvent(const Engine::Event&)
{

}