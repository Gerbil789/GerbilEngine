#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Audio/AudioClip.h"

namespace Engine
{
	class AudioImporter
	{
	public:
		static Ref<AudioClip> ImportAudio(const AssetMetadata& metadata);
		static Ref<AudioClip> LoadAudio(const std::filesystem::path& path);
	};
}