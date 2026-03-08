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

using namespace Engine;

void PlayerController::OnCreate() {}

void PlayerController::OnUpdate()
{
	float delta = Time::DeltaTime();

	auto& transform = Self.GetComponent<TransformComponent>();

	// movement
	if (Input::IsKeyDown(KeyCode::A)) transform.position.x -= m_MoveSpeed * delta;
	if (Input::IsKeyDown(KeyCode::D)) transform.position.x += m_MoveSpeed * delta;
	if (Input::IsKeyDown(KeyCode::W)) transform.position.z += m_MoveSpeed * delta;
	if (Input::IsKeyDown(KeyCode::S)) transform.position.z -= m_MoveSpeed * delta;
	if (Input::IsKeyDown(KeyCode::Space)) transform.position.y += m_JumpStrength * delta;

	// gravity
	if(transform.position.y > m_Ground)
	{
		transform.position.y -= Time::DeltaTime() * m_GravityStrength;
	}
	else
	{
		transform.position.y = m_Ground;
	}

	// shoot
	if(Input::IsKeyPressedOnce(KeyCode::M))
	{
		if (!m_Sound || !m_BulletMesh || !m_BulletMaterial)
		{
			LOG_WARNING("Missing shoot components!");
			return;
		}

		auto scene = SceneManager::GetActiveScene();

		auto pos = Self.GetComponent<TransformComponent>().position;
		Audio::Play3D(m_Sound, pos.x, pos.y, pos.z);


		Entity bullet = scene->CreateEntity("Bullet");
		bullet.GetComponent<TransformComponent>().position = pos;
		bullet.AddComponent<MeshComponent>(m_BulletMesh).SetMaterial(0, m_BulletMaterial);

		auto& scriptComponent = bullet.AddComponent<ScriptComponent>();
		scriptComponent.id = "Bullet";
		scriptComponent.instance = new Bullet(glm::vec3(0.0f, 0.0f, 1.0f));
		scriptComponent.instance->Self = bullet;

	}
}


void Bullet::OnUpdate()
{
	auto& transform = Self.GetComponent<TransformComponent>();

	transform.position += m_Direction * m_Speed * Time::DeltaTime();

	//m_Time -= Time::DeltaTime();

	//if(m_Time <= 0.0f)
	//{
	//	Self.RemoveComponent<Engine::MeshComponent>();
	//	Self.RemoveComponent<Engine::MaterialComponent>();
	//}
}
