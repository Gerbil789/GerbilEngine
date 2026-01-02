#include "enginepch.h"
#include "AudioClip.h"
#include <miniaudio.h>   

namespace Engine
{
	ma_uint64 m_FrameCount = 0;

	AudioClip::AudioClip(const std::string& path)
	{
		ma_decoder decoder;
		if (ma_decoder_init_file(path.c_str(), NULL, &decoder) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to load audio.");
		}

		m_Channels = decoder.outputChannels;
		m_SampleRate	 = decoder.outputSampleRate;

		// Load all PCM frames into memory
		m_FrameCount = 0;

		if (ma_decoder_get_length_in_pcm_frames(&decoder, &m_FrameCount) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to get audio length.");
		}


		m_PCMFrames.resize(m_FrameCount * m_Channels);
		ma_decoder_read_pcm_frames(&decoder, m_PCMFrames.data(), m_FrameCount, NULL);

		ma_decoder_uninit(&decoder);
	}

	uint64_t AudioClip::GetTotalFrames() const
	{
		return static_cast<uint64_t>(m_FrameCount);
	}
}