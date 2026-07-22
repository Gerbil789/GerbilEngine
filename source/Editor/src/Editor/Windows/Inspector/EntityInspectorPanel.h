#pragma once

#include "Engine/Core/UUID.h"

namespace Editor
{
	class EntityInspectorPanel
	{
	public:
		void Draw(Engine::Uuid entityId);
	};
}