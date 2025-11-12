#pragma once

#include "Engine/Renderer/Material.h"

namespace Editor
{
	class MaterialInspectorPanel
	{
	public:
		void Draw(Ref<Engine::Material> material);

	};
}