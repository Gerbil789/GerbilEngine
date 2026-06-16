#include "Button.h"
#include "Engine/Scene/Components.h"
#include "Engine/Core/Log.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Time.h"

void Button::OnUpdate()
{
	if (!m_Pressed) return;

	auto delta = Engine::Time::DeltaTime() * m_Speed;
	auto& transform = GetComponent<Engine::TransformComponent>();
	transform.rotation.y += delta;
	m_Scene->GetRegistry().patch<Engine::TransformComponent>(m_Entity);
}

void Button::OnTriggerEnter(entt::entity other)
{
	Engine::TransformComponent& transform = GetComponent<Engine::TransformComponent>();
	Engine::Audio::Play3D(&m_PressSound.Get(), transform.position);
	m_Pressed = true;
	LOG_TRACE("Button pressed by entity {}", (uint32_t)other);
}

void Button::OnTriggerExit(entt::entity other)
{
	Engine::TransformComponent& transform = GetComponent<Engine::TransformComponent>();
	Engine::Audio::Play3D(&m_ReleaseSound.Get(), transform.position);
	m_Pressed = false;
	LOG_TRACE("Button released by entity {}", (uint32_t)other);
}
