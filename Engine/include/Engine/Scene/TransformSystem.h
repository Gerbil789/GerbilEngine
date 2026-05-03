#pragma once

#include "Engine/Core/API.h"
#include <entt.hpp>

namespace Engine
{
	class ENGINE_API TransformSystem
	{
	public:
		static void SetScene();
		static void Update();
	};
}