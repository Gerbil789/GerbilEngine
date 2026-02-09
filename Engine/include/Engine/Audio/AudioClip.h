#pragma once

#include "Engine/Asset/Asset.h"
#include <vector>
#include <string>

namespace Engine
{
	class AudioClip : public Asset
	{
	public:
		AudioClip(const std::string& path);

		uint32_t GetChannels() const { return m_Channels; }
		uint32_t GetSampleRate() const { return m_SampleRate; }
		uint64_t GetTotalFrames() const;
		const std::vector<float>& GetPCMFrames() const { return m_PCMFrames; }

	private:
		uint32_t m_Channels = 0;  // 1 = mono, 2 = stereo
		uint32_t m_SampleRate = 0;
		std::vector<float> m_PCMFrames;
	};
}