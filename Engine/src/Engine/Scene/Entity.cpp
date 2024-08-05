#include "enginepch.h"
#include "Entity.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
		Entity::Entity(entt::entity handle, Scene* scene): m_EntityHandle(handle), m_Scene(scene) {}
}