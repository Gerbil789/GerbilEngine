#include "enginepch.h"
#include "Engine/Asset/Importer/AudioImporter.h"

namespace engine
{
	AudioClip AudioImporter::LoadAudioClip(const std::filesystem::path& path)
	{
		return AudioClip(path);
	}
}