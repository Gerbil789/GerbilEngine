#pragma once

#include "Engine/Core/UUID.h"

namespace editor
{
	class EntityInspectorPanel
	{
	public:
		void Draw(engine::Uuid entityId);
	};
}