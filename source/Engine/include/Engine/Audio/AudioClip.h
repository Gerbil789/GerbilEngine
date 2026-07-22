#pragma once

#include "Engine/Asset/Asset.h"
#include <filesystem>
#include <vector>

namespace Engine
{
	class ENGINE_API AudioClip : public Asset
	{
	public:
		AudioClip(const std::filesystem::path& path);

		float GetDurationSeconds() const;
		const float* GetPCMData() const { return m_PCMData.data(); }
		uint64_t GetTotalFrames() const { return m_TotalFrames; }
		uint32_t GetChannels() const { return m_Channels; }
		uint32_t GetSampleRate() const { return m_SampleRate; }

	private:
		std::vector<float> m_PCMData;
		uint64_t m_TotalFrames = 0;
		uint32_t m_Channels = 0;
		uint32_t m_SampleRate = 0;
	};
}