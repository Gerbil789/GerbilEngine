#pragma once

#include "Engine/Asset/Handle.h"

namespace editor
{
	class ShaderInspectorPanel
	{
	public:
		void Draw(engine::ShaderHandle handle);
	};
}