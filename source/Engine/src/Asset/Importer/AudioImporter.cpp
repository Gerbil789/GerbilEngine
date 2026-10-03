#include "enginepch.h"
#include "Engine/Asset/Importer/AudioImporter.h"

namespace engine
{
	AudioClipAsset AudioImporter::LoadAudioClip(const std::filesystem::path& path)
	{
		return AudioClipAsset(path);
	}
}