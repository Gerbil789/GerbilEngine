#include "PlayerController.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Time.h"

using namespace Engine;

void PlayerController::OnCreate()
{
	//auto& component = Self.AddComponent<AudioSourceComponent>();
	//AudioClip* clip = AssetManager::GetAsset<AudioClip>(6350057269135839349);
	//if(!clip)
	//{
	//	LOG_ERROR("Audio clip not found");
	//	return;
	//}

	//component.clip = clip;
}

void PlayerController::OnUpdate()
{
	float delta = Time::DeltaTime();

	auto& transform = Self.GetComponent<TransformComponent>();

	// movement
	if (Input::IsKeyPressed(Key::A)) transform.position.x -= m_MoveSpeed * delta;
	if (Input::IsKeyPressed(Key::D)) transform.position.x += m_MoveSpeed * delta;
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




	if (Input::IsKeyPressed(Key::Tab))
	{
		Material* mat = Self.GetComponent<MeshComponent>().material;
		mat->SetTexture("Albedo", m_Texture);

		LOG_INFO("Tab pressed, setting material");
	}

	if (Input::IsKeyPressed(Key::P))
	{
		LOG_TRACE("Playing audio");
		Audio::Play(m_Clip);
		//Self.GetComponent<AudioSourceComponent>().Play();
	}

	if (Input::IsKeyPressed(Key::O))
	{
		LOG_TRACE("Stopping audio");
		Audio::Stop(m_Clip);
		//Self.GetComponent<AudioSourceComponent>().Stop();
	}

}

