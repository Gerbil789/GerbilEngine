#include "enginepch.h"
#include "AudioImporter.h"
#include "Engine/Core/Project.h"
#include "Engine/Audio/Audio.h"

namespace Engine
{
	Ref<AudioClip> AudioImporter::ImportAudio(const AssetMetadata& metadata)
	{
		return LoadAudio(Project::GetAssetsDirectory() / metadata.path);
	}

	Ref<AudioClip> AudioImporter::LoadAudio(const std::filesystem::path& path)
	{
		return CreateRef<AudioClip>(path.string());
	}
}