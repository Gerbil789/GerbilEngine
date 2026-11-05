#pragma once

#include "Engine/Audio/AudioClip.h"

namespace editor
{
	class AudioInspectorPanel
	{
	public:
		void Draw(engine::AudioClipHandle clip);
	};
}