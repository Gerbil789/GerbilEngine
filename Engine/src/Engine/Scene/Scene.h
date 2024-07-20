#pragma once

#include "entt.hpp"

namespace Engine 
{
	class Scene
	{
	public:
		Scene();
		~Scene();

	private:
		entt::registry m_Registry;
	
	};

}


