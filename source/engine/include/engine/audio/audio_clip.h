#pragma once

#include <filesystem>
#include <vector>

namespace engine
{
	struct AudioClip
	{
		std::vector<float> PCMData;
		uint64_t totalFrames = 0;
		uint32_t channels = 0;
		uint32_t sampleRate = 0;

		[[nodiscard]] static AudioClip Create(const std::filesystem::path& path);

		AudioClip() = default;

		AudioClip(const AudioClip&) = delete;
		AudioClip& operator=(const AudioClip&) = delete;

		AudioClip(AudioClip&&) noexcept = default;
		AudioClip& operator=(AudioClip&&) noexcept = default;
	};
}