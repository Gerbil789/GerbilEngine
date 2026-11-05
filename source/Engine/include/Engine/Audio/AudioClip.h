#pragma once

#include <filesystem>
#include <vector>

namespace engine
{
	struct AudioClip
	{
		AudioClip() = default;
		AudioClip(const std::filesystem::path& path);

		std::vector<float> PCMData{};
		uint64_t totalFrames = 0;
		uint32_t channels = 0;
		uint32_t sampleRate = 0;
	};
}