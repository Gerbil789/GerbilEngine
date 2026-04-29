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

AABB CalculateWorldAABB(const AABB& local, const glm::mat4& transformMatrix) {
	// The 8 corners of the local bounding box
	glm::vec3 corners[8] = {
			{local.min.x, local.min.y, local.min.z},
			{local.max.x, local.min.y, local.min.z},
			{local.min.x, local.max.y, local.min.z},
			{local.max.x, local.max.y, local.min.z},
			{local.min.x, local.min.y, local.max.z},
			{local.max.x, local.min.y, local.max.z},
			{local.min.x, local.max.y, local.max.z},
			{local.max.x, local.max.y, local.max.z}
	};

	glm::vec3 worldMin(FLT_MAX);
	glm::vec3 worldMax(-FLT_MAX);

	for (int i = 0; i < 8; ++i) 
	{
		glm::vec3 worldPos = glm::vec3(transformMatrix * glm::vec4(corners[i], 1.0f));
		worldMin = glm::min(worldMin, worldPos);
		worldMax = glm::max(worldMax, worldPos);
	}

	return { worldMin, worldMax };
}


bool IntersectTest(const AABB& a, const AABB& b)
{
	return
		(a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z);

}

void CheckCollision(const Entity& self)
{
	Uuid id = self.Get<MeshComponent>().mesh;
	if(!id)
	{
		LOG_WARNING("Player does not have mesh!");
		return;
	}

	Mesh& playerMesh = AssetManager::GetAsset<Mesh>(id);
	const TransformComponent& playerTransform = self.Get<TransformComponent>();
	
	AABB playerWorldAABB = CalculateWorldAABB(playerMesh.aabb, playerTransform.GetWorldMatrix());

	Scene& scene = SceneManager::GetActiveScene();
	auto entities = scene.GetEntities<MeshComponent>();

	int count = 0;

	for(const auto& entity : entities)
	{
		if (entity == self) continue;

		Uuid meshID = entity.Get<MeshComponent>().mesh;
		Mesh& mesh = AssetManager::GetAsset<Mesh>(meshID);
		const TransformComponent& transform = entity.Get<TransformComponent>();
		AABB worldAABB = CalculateWorldAABB(mesh.aabb, transform.GetWorldMatrix());

		if(IntersectTest(playerWorldAABB, worldAABB))
		{
			count++;
		}
	}

	LOG_INFO("Intersect count: {}", count);
}

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

	CheckCollision(Self);
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