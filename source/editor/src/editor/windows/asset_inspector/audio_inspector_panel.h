#pragma once

#include "engine/asset/handle.h"

namespace editor
{
	struct AudioInspectorPanel
	{
		void Draw(engine::AudioClipHandle handle);
	};
}