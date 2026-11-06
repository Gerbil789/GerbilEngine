#include "FPSPlayerController.h"
//#include "Engine/Core/Input.h"
//#include "Engine/Core/Log.h"
//#include "Engine/Core/Time.h"
//
//using namespace Engine;
//
//void FPSPlayerController::OnStart()
//{
//	Input::SetCursorMode(Input::CursorMode::Disabled);
//	m_lastMouseX = Input::GetMousePosition().x;
//	m_lastMouseY = Input::GetMousePosition().y;
//}
//
//void FPSPlayerController::OnUpdate()
//{
//	float delta = Time::DeltaTime();
//	auto& transform = m_entity.GetComponent<TransformComponent>();
//
//	float yawRadians = glm::radians(transform.rotation.y);
//
//	float forwardX = std::sin(yawRadians);
//	float forwardZ = std::cos(yawRadians);
//
//	float rightX = std::cos(yawRadians);
//	float rightZ = -std::sin(yawRadians);
//
//	// movement
//	if (Input::IsKeyDown(Key::W))
//	{
//		transform.position.x += forwardX * m_moveSpeed * delta;
//		transform.position.z += forwardZ * m_moveSpeed * delta;
//	}
//	if (Input::IsKeyDown(Key::S))
//	{
//		transform.position.x -= forwardX * m_moveSpeed * delta;
//		transform.position.z -= forwardZ * m_moveSpeed * delta;
//	}
//	if (Input::IsKeyDown(Key::D))
//	{
//		transform.position.x += rightX * m_moveSpeed * delta;
//		transform.position.z += rightZ * m_moveSpeed * delta;
//	}
//	if (Input::IsKeyDown(Key::A))
//	{
//		transform.position.x -= rightX * m_moveSpeed * delta;
//		transform.position.z -= rightZ * m_moveSpeed * delta;
//	}
//
//	// gravity
//	m_velocityY -= m_gravityStrength * delta;
//	transform.position.y += m_velocityY * delta;
//	if (transform.position.y <= m_ground)
//	{
//		transform.position.y = m_ground; 
//		m_velocityY = 0.0f;
//	}
//}
//
//void FPSPlayerController::OnDestroy()
//{
//
//}
//
//void FPSPlayerController::OnEvent(const Engine::Event&)
//{
//
//}