#include "PlayerController.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"

using namespace Engine;

void PlayerController::OnCreate()
{
	auto& component = Self.AddComponent<AudioSourceComponent>();
	AudioClip* clip = AssetManager::GetAsset<AudioClip>(6350057269135839349);
	if(!clip)
	{
		LOG_ERROR("Audio clip not found");
		return;
	}

	component.clip = clip;
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

	if (Input::IsKeyPressed(Key::P))
	{
		LOG_TRACE("Playing audio");
		Self.GetComponent<AudioSourceComponent>().Play();
	}

	if (Input::IsKeyPressed(Key::O))
	{
		LOG_TRACE("Stopping audio");
		Self.GetComponent<AudioSourceComponent>().Stop();
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

