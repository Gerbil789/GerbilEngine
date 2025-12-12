#pragma once

#include "Engine/Asset/Asset.h"
#include <miniaudio.h>   

namespace Engine
{
	class AudioClip : public Asset
	{
	public:
		AudioClip(const std::string& path)
		{
			ma_decoder decoder;
			if (ma_decoder_init_file(path.c_str(), NULL, &decoder) != MA_SUCCESS)
			{
				LOG_ERROR("Failed to load audio.");
			}

			m_channels = decoder.outputChannels;
			m_sampleRate = decoder.outputSampleRate;
			m_format = decoder.outputFormat;

			// Load all PCM frames into memory
			ma_uint64 frameCount = 0;

			if(ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) != MA_SUCCESS)
			{
				LOG_ERROR("Failed to get audio length.");
			}


			m_pcmFrames.resize(frameCount * m_channels);
			ma_decoder_read_pcm_frames(&decoder, m_pcmFrames.data(), frameCount, NULL);

			ma_decoder_uninit(&decoder);

			m_totalFrames = frameCount;

			GenerateWaveformPreview(2000);
		}

		void GenerateWaveformPreview(uint32_t resolution)
		{
			const uint64_t totalFrames = m_totalFrames;
			const uint32_t channels = m_channels;
			const float* pcm = m_pcmFrames.data();

			m_waveformMin.resize(resolution);
			m_waveformMax.resize(resolution);

			uint64_t framesPerBucket = totalFrames / resolution;

			for (uint32_t i = 0; i < resolution; i++)
			{
				uint64_t start = i * framesPerBucket;
				uint64_t end = (i + 1) * framesPerBucket;
				if (end > totalFrames) end = totalFrames;

				float minVal = 1.0f;
				float maxVal = -1.0f;

				for (uint64_t f = start; f < end; f++)
				{
					// Average all channels
					float sample = 0.0f;
					for (uint32_t c = 0; c < channels; c++)
						sample += pcm[f * channels + c];
					sample /= channels;

					if (sample < minVal) minVal = sample;
					if (sample > maxVal) maxVal = sample;
				}

				m_waveformMin[i] = minVal;
				m_waveformMax[i] = maxVal;
			}
		}

		const float* GetWaveformMin() const { return m_waveformMin.data(); }
		const float* GetWaveformMax() const { return m_waveformMax.data(); }
		const int GetWaveformResolution() const { return static_cast<int>(m_waveformMin.size()); }

		const std::vector<float>& GetPCMFrames() const { return m_pcmFrames; }
		uint32_t GetChannels() const { return m_channels; }
		uint32_t GetSampleRate() const { return m_sampleRate; }
		ma_uint64 GetTotalFrames() const { return m_totalFrames; }

	private:
		std::vector<float> m_pcmFrames;
		uint32_t m_channels = 0;
		uint32_t m_sampleRate = 0;
		ma_format m_format;
		ma_uint64 m_totalFrames = 0;

		std::vector<float> m_waveformMin;
		std::vector<float> m_waveformMax;
	};
}