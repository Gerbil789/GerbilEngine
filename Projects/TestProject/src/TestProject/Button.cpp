#include "Button.h"
#include "Engine/Scene/Components.h"
#include "Engine/Core/Log.h"
#include "Engine/Audio/Audio.h"

void Button::OnTriggerEnter(entt::entity other)
{
	Engine::TransformComponent& transform = GetComponent<Engine::TransformComponent>();
	//Engine::TransformComponent& meshTransform = transform.firstChild.GetComponent<Engine::TransformComponent>();
	//meshTransform.position.y += m_Offset;
	//Engine::Audio::Play3D(m_PressSound, transform.position);

	//LOG_TRACE("Button pressed by entity {}", other.GetName());
}

void Button::OnTriggerExit(entt::entity other)
{
	//Engine::TransformComponent& transform = Self.Get<Engine::TransformComponent>();
	//Engine::TransformComponent& meshTransform = transform.firstChild.Get<Engine::TransformComponent>();
	//meshTransform.position.y -= m_Offset;
	//glm::vec3& pos = transform.position;
	//Engine::Audio::Play3D(m_ReleaseSound, transform.position);

	//LOG_TRACE("Button released by entity {}", other.GetName());
}
