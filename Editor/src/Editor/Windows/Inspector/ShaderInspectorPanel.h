#pragma once

#include "Engine/Graphics/Shader.h"
#include "Engine/Core/Core.h"

namespace Editor
{
	class ShaderInspectorPanel
	{
	public:
		void Draw(Ref<Engine::Shader> shader);
	};
}