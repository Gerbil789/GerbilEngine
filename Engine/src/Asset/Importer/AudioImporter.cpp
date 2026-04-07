#include "enginepch.h"
#include "Engine/Asset/Importer/AudioImporter.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"

namespace Engine
{
	AudioClip* AudioImporter::ImportAudio(const std::filesystem::path& path)
	{
		return LoadAudio(path);
	}

	AudioClip* AudioImporter::LoadAudio(const std::filesystem::path& path)
	{
		return new AudioClip(path);
	}
}