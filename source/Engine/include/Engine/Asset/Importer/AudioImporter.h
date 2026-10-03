#pragma once

#include "Engine/Audio/AudioClip.h"

namespace engine
{
	class AudioImporter
	{
	public:
		static AudioClipAsset LoadAudioClip(const std::filesystem::path& path);
	};
}