#pragma once

#include "Engine/Audio/AudioClip.h"
#include "Engine/Core/Core.h"

namespace Editor
{
	class AudioInspectorPanel
	{
	public:
		void Draw(Engine::AudioClip* audioClip);
	};
}