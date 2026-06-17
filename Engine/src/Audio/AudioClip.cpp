#include "enginepch.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Core/Log.h"
#include <miniaudio.h>   

namespace Engine
{
	AudioClip::AudioClip(const std::filesystem::path& path)
	{
		ma_decoder decoder;
		// 0, 0 means "use the file's native channel count and sample rate"
		ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);

		if (ma_decoder_init_file(path.string().c_str(), &config, &decoder) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to load audio file: {0}", path.string());
			return;
		}

		m_Channels = decoder.outputChannels;
		m_SampleRate = decoder.outputSampleRate;

		ma_decoder_get_length_in_pcm_frames(&decoder, &m_TotalFrames);

		m_PCMData.resize(m_TotalFrames * m_Channels);
		ma_decoder_read_pcm_frames(&decoder, m_PCMData.data(), m_TotalFrames, nullptr);

		ma_decoder_uninit(&decoder);
	}

	float AudioClip::GetDurationSeconds() const
	{
		if (m_SampleRate == 0) return 0.0f;
		return static_cast<float>(m_TotalFrames) / static_cast<float>(m_SampleRate);
	}
}