#pragma once

namespace Engine
{
	class AudioClipAsset;

	class ENGINE_API AudioImporter
	{
	public:
		static std::optional<AudioClipAsset> LoadAudioClip(const std::filesystem::path& path);
	};
}