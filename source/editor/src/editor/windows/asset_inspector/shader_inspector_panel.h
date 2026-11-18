#pragma once

#include "engine/asset/handle.h"

namespace editor
{
	class ShaderInspectorPanel
	{
	public:
		void Draw(engine::ShaderHandle handle);
	};
}