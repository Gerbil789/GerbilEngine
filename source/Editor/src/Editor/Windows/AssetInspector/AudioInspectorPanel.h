#pragma once

#include "Engine/Asset/Handle.h"

namespace editor
{
	class AudioInspectorPanel
	{
	public:
		void Draw(engine::AudioClipHandle handle);
	};
}