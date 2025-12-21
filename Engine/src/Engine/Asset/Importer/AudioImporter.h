#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Audio/AudioClip.h"

namespace Engine
{
	class AudioImporter
	{
	public:
		static AudioClip* ImportAudio(const AssetMetadata& metadata);
		static AudioClip* LoadAudio(const std::filesystem::path& path);
	};
}