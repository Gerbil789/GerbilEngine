#include "enginepch.h"
#include "AudioImporter.h"
#include "Engine/Core/Project.h"
#include "Engine/Audio/Audio.h"

namespace Engine
{
	AudioClip* AudioImporter::ImportAudio(const AssetMetadata& metadata)
	{
		return LoadAudio(Project::GetAssetsDirectory() / metadata.path);
	}

	AudioClip* AudioImporter::LoadAudio(const std::filesystem::path& path)
	{
		return new AudioClip(path.string());
	}
}