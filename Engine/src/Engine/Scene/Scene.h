#pragma once

#include "entt.hpp"
#include "Engine/Core/Timestep.h"

namespace Engine 
{
	class Entity; // Forward declaration

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep ts);

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);


	private:
		entt::registry m_Registry;

		friend class Entity;
	
	};
}