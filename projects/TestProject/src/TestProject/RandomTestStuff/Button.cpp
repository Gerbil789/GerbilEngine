#include "Button.h"
#include "Engine/Core/Components.h"
#include "Engine/Core/Log.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Time.h"

void Button::OnUpdate()
{
	if (!m_Pressed) return;

	auto delta = Engine::Time::DeltaTime() * m_Speed;
	auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
	transform.rotation.y += delta;
}

void Button::OnTriggerEnter(Engine::Entity other)
{
	Engine::TransformComponent& transform = m_Entity.GetComponent<Engine::TransformComponent>();
	Engine::Audio::Play3D(m_PressSound, transform.position);
	m_Pressed = true;
	LOG_TRACE("Button pressed by entity {}", static_cast<uint32_t>(other.GetHandle()));
}

void Button::OnTriggerExit(Engine::Entity other)
{
	Engine::TransformComponent& transform = m_Entity.GetComponent<Engine::TransformComponent>();
	Engine::Audio::Play3D(m_ReleaseSound, transform.position);
	m_Pressed = false;
	LOG_TRACE("Button released by entity {}", static_cast<uint32_t>(other.GetHandle()));
}
