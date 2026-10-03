#pragma once

#include "Engine/Asset/AssetHandle.h"
#include <filesystem>
#include <vector>

namespace engine
{
	struct AudioClipAsset
	{
		AudioClipAsset(const std::filesystem::path& path);

		std::vector<float> PCMData;
		uint64_t totalFrames = 0;
		uint32_t channels = 0;
		uint32_t sampleRate = 0;
	};

	struct AudioClip : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		AudioClipAsset& Get() const;

		float GetDurationSeconds() const;
		const float* GetPCMData() const;
		uint64_t GetTotalFrames() const;
		uint32_t GetChannels() const;
		uint32_t GetSampleRate() const;
	};
}