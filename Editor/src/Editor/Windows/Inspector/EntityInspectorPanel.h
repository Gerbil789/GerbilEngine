#pragma once

#include "Engine/Scene/Entity.h"

namespace Editor
{
	class EntityInspectorPanel
	{
	public:
		void Draw(Engine::Entity entity);

	private:
		void DrawComponents(Engine::Entity entity);
		void DrawAddComponentButton(Engine::Entity entity);
	};
}