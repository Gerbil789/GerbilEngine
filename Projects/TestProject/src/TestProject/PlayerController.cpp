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
#include "Engine/Scene/SceneManager.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/Event.h"

using namespace Engine;

void PlayerController::OnStart()
{
	Input::SetCursorMode(Input::CursorMode::Disabled);
	m_LastMouseX = Input::GetMousePosition().x;
	m_LastMouseY = Input::GetMousePosition().y;
}

void PlayerController::OnUpdate()
{
	float delta = Time::DeltaTime();
	auto& transform = Self.Get<TransformComponent>();

	float yawRadians = glm::radians(transform.rotation.y);

	float forwardX = std::sin(yawRadians);
	float forwardZ = std::cos(yawRadians);

	float rightX = std::cos(yawRadians);
	float rightZ = -std::sin(yawRadians);

	// movement
	if (Input::IsKeyDown(KeyCode::W))
	{
		transform.position.x += forwardX * m_MoveSpeed * delta;
		transform.position.z += forwardZ * m_MoveSpeed * delta;
	}
	if (Input::IsKeyDown(KeyCode::S))
	{
		transform.position.x -= forwardX * m_MoveSpeed * delta;
		transform.position.z -= forwardZ * m_MoveSpeed * delta;
	}
	if (Input::IsKeyDown(KeyCode::D))
	{
		transform.position.x += rightX * m_MoveSpeed * delta;
		transform.position.z += rightZ * m_MoveSpeed * delta;
	}
	if (Input::IsKeyDown(KeyCode::A))
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
}

void PlayerController::OnDestroy()
{

}

void PlayerController::OnEvent(const Engine::Event& event)
{
	if(event.GetEventType() == EventType::MouseMoved)
	{
		const auto& e = static_cast<const MouseMovedEvent&>(event);

		float xOffset = e.GetX() - m_LastMouseX;
		float yOffset = m_LastMouseY - e.GetY();

		m_LastMouseX = e.GetX();
		m_LastMouseY = e.GetY();

		auto& transform = Self.Get<TransformComponent>();

		transform.rotation.y += xOffset * m_MouseSensitivity;
		transform.rotation.x -= yOffset * m_MouseSensitivity;

		if (transform.rotation.x > 89.0f) transform.rotation.x = 89.0f;
		if (transform.rotation.x < -89.0f) transform.rotation.x = -89.0f;
	}

	if (event.GetEventType() == EventType::KeyPressed)
	{
		const auto& e = static_cast<const KeyPressedEvent&>(event);
		auto& transform = Self.Get<TransformComponent>();
		bool isGrounded = (transform.position.y <= m_Ground);

		if(!isGrounded)
		{
			return;
		}

		if (e.GetKey() == KeyCode::Space)
		{
			m_VelocityY = m_JumpStrength;
		}
	}
}