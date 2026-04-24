#include "enginepch.h"
#include "Engine/Asset/Importer/AudioImporter.h"
#include "Engine/Audio/AudioClip.h"

namespace Engine
{
	std::optional<AudioClip> AudioImporter::LoadAudioClip(const std::filesystem::path& path)
	{
		return AudioClip(path);
	}
}