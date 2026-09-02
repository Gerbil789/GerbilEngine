#include "enginepch.h"
#include "Engine/Asset/Importer/AudioImporter.h"
#include "Engine/Audio/AudioClip.h"

namespace engine
{
	std::optional<AudioClipAsset> AudioImporter::LoadAudioClip(const std::filesystem::path& path)
	{
		return AudioClipAsset(path);
	}
}