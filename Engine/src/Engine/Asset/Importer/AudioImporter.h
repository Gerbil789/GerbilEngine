#pragma once

#include "Engine/Asset/AssetRecord.h"
#include "Engine/Audio/AudioClip.h"

namespace Engine
{
	class AudioImporter
	{
	public:
		static AudioClip* ImportAudio(const AssetRecord& metadata);
		static AudioClip* LoadAudio(const std::filesystem::path& path);
	};
}