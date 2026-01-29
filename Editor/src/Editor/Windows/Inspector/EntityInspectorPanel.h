#pragma once

namespace Engine { class Entity; }

namespace Editor
{
	class EntityInspectorPanel
	{
	public:
		void Draw(Engine::Entity entity);
	};
}