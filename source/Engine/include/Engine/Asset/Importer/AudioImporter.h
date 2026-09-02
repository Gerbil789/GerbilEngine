#pragma once

namespace engine
{
	class AudioClipAsset;

	class AudioImporter
	{
	public:
		static std::optional<AudioClipAsset> LoadAudioClip(const std::filesystem::path& path);
	};
}