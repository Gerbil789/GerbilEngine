#pragma once

namespace Engine
{
	class AudioClipAsset;

	class AudioImporter
	{
	public:
		static std::optional<AudioClipAsset> LoadAudioClip(const std::filesystem::path& path);
	};
}