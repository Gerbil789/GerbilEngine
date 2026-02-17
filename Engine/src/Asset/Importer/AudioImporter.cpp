#include "enginepch.h"
#include "Engine/Asset/Importer/AudioImporter.h"
#include "Engine/Core/Engine.h"
#include "Engine/Audio/Audio.h"

namespace Engine
{
	AudioClip* AudioImporter::ImportAudio(const AssetRecord& metadata)
	{
		return LoadAudio(Engine::GetAssetsDirectory() / metadata.path);
	}

	AudioClip* AudioImporter::LoadAudio(const std::filesystem::path& path)
	{
		return new AudioClip(path);
	}
}