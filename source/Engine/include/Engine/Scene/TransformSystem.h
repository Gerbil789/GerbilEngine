#pragma once

#include "Engine/Core/API.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class ENGINE_API TransformSystem
	{
	public:
		static void Update(Scene& scene);
	};
}