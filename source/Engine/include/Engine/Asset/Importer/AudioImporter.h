#pragma once

#include "Engine/Audio/AudioClip.h"

namespace engine
{
	class AudioImporter
	{
	public:
		static AudioClip LoadAudioClip(const std::filesystem::path& path);
	};
}